# Declarations

## Anatomy of a declaration

Every declaration follows this pattern:

```
[storage-class] [qualifier...] type declarator [= initializer];
```

```c
static const int max_retries = 5;
//  │       │    │   │            └─ initializer
//  │       │    │   └─ declarator (name, pointers, arrays, functions)
//  │       │    └─ base type
//  │       └─ type qualifier
//  └─ storage class
```

You can declare multiple in one statement, but each declarator gets its own pointer/array decoration:

```c
int *p, q;      // p is int*, q is int — the * binds to p, not to int
int *p, *q;     // both are int*
```

This is why many style guides put one declaration per line.

## Storage classes

Only one storage class per declaration. Most of the time you use none (which means `auto` for locals, `extern` for file-scope).

| Keyword | Where | What it does |
|---------|-------|-------------|
| `auto` | block scope | default for locals — explicitly says "automatic lifetime." You never write this. |
| `static` | block scope | local variable that persists across calls (initialized once) |
| `static` | file scope | restricts visibility to this translation unit (internal linkage) |
| `extern` | file/block scope | declares without defining — "this is defined somewhere else" |
| `register` | block scope | hint to keep in a register. Compiler ignores it. You can't take the address (`&`) of a `register` variable. |

### `static` at block scope — persistent local

```c
int next_id(void) {
    static int id = 0;  // initialized to 0 once, survives between calls
    return ++id;        // 1, 2, 3, ... on successive calls
}
```

- initialized once, at program start (not at the `static` line)
- if no initializer, implicitly 0 (unlike `auto` locals which are garbage)
- lifetime = entire program, scope = just this block

### `static` at file scope — internal linkage

```c
// helpers.c
static int count = 0;           // only visible in this file
static void reset(void) { count = 0; }  // only callable from this file
```

### `extern` — declaration without definition

```c
// config.h
extern int debug_level;         // "this exists, defined elsewhere"

// config.c
int debug_level = 0;            // actual definition — memory allocated here
```

- `extern` with an initializer IS a definition: `extern int x = 5;` — the `extern` is pointless here
- function declarations are implicitly `extern` — `void foo(void);` and `extern void foo(void);` are identical

## Type qualifiers

Qualifiers can be combined. They restrict what you can do with a variable.

### `const` — "I promise not to modify this"

```c
const int max = 100;
max = 200;                  // compile error

const int *p = &max;        // pointer to const int — can't modify *p
int *const q = &x;          // const pointer to int — can't modify q itself
const int *const r = &max;  // const pointer to const int — can't modify either
```

Read right to left: `const int *p` → "p is a pointer to int that is const"

**`const` is not a compile-time constant in C** (unlike C++):
```c
const int n = 10;
int arr[n];                 // VLA in C99+, error in C89. NOT a static array.
```

For true compile-time constants, use `#define` or `enum`:
```c
#define MAX 100
enum { MAX = 100 };         // enum constants are always int
```

### `volatile` — "this can change behind the compiler's back"

```c
volatile int *status_reg = (volatile int *)0x40021000;
```

Tells the compiler: **do not optimize reads/writes to this variable.** Every access in the source code must result in an actual memory access.

Without `volatile`, the compiler may:
- cache a register value in a CPU register and never re-read from memory
- reorder or eliminate reads/writes it thinks are redundant
- "optimize away" a loop that polls a variable

**When you need `volatile`:**
- hardware registers (memory-mapped I/O) — the register changes because hardware changed it
- variables modified by an ISR (interrupt service routine) — the ISR runs asynchronously
- variables shared between threads (though `volatile` alone is NOT enough for thread safety — you also need atomic operations or mutexes)

```c
// embedded: polling a hardware status register
volatile uint32_t *UART_SR = (volatile uint32_t *)0x40011000;
while (!(*UART_SR & (1 << 5))) {
    // wait for RXNE bit — without volatile, compiler might read once and loop forever
}

// ISR-modified flag
volatile int data_ready = 0;

void USART2_IRQHandler(void) {
    data_ready = 1;              // set by interrupt
}

int main(void) {
    while (!data_ready) { }      // without volatile: compiler sees data_ready is 0,
                                 // "optimizes" to while(1) — infinite loop
    process_data();
}
```

**`const volatile`** — sounds contradictory but makes sense:
```c
const volatile uint32_t *UART_DR = (const volatile uint32_t *)0x40011004;
// const: your code must not write to it
// volatile: the hardware can change it — compiler must re-read every time
```

### `restrict` (C99) — "I'm the only pointer to this memory"

```c
void copy(int *restrict dst, const int *restrict src, size_t n) {
    for (size_t i = 0; i < n; i++)
        dst[i] = src[i];
}
```

- a promise to the compiler that `dst` and `src` don't overlap
- enables better optimization (vectorization, reordering)
- violating the promise is undefined behavior — your bug, not the compiler's
- `memcpy` uses `restrict`, `memmove` doesn't (because memmove handles overlap)

## Reading complex declarations

Declarations in C are read from the declarator name outward, alternating right then left:

```c
int *p;                     // p is a pointer to int
int a[10];                  // a is an array of 10 ints
int f(void);                // f is a function returning int
int *a[10];                 // a is an array of 10 pointers to int
int (*p)[10];               // p is a pointer to an array of 10 ints
int (*fp)(int, int);        // fp is a pointer to a function (int, int) → int
int *(*fp)(int);            // fp is a pointer to a function (int) → pointer to int
```

**The right-left rule:**
1. Start at the name
2. Go right until you hit `)` or end of declarator
3. Go left until you hit `(` or start of declarator
4. Repeat

```c
int (*(*fp)(int))[10];
//       fp                  — fp is
//         (int)             — a function taking int
//      (*    )              — returning a pointer to
//             [10]          — an array of 10
//  int                      — ints
// ... returning pointer to array of 10 ints? → use a pointer to pointer to function
```

When declarations get this complex, `typedef` saves you:
```c
typedef int TenInts[10];
typedef TenInts *PtrToTenInts;
typedef PtrToTenInts (*FuncPtr)(int);
FuncPtr fp;                 // same as: int (*(*fp)(int))[10]
```

## Initializers

### Scalar initialization

```c
int x = 42;                // simple
int y = 3 + 4;             // expression — evaluated at runtime for locals
static int z = 3 + 4;      // must be a constant expression for static/file-scope
```

### Array initialization

```c
int a[5] = {1, 2, 3, 4, 5};
int b[5] = {1, 2};              // b = {1, 2, 0, 0, 0} — rest zero-filled
int c[] = {1, 2, 3};            // compiler infers size = 3
int d[5] = {0};                 // all zeros — the zero-init idiom
```

### Designated initializers (C99)

```c
int a[10] = {[3] = 30, [7] = 70};      // a[3]=30, a[7]=70, rest 0
int b[] = {[9] = 1};                    // compiler infers size = 10

struct Point { int x, y, z; };
struct Point p = {.y = 5, .x = 3};     // order doesn't matter, z = 0
```

### Struct and nested initialization

```c
struct Rect {
    struct Point tl, br;
};

struct Rect r = {{0, 0}, {100, 200}};              // positional
struct Rect r = {.tl = {.x = 0, .y = 0}, .br.x = 100, .br.y = 200};  // designated
```

### Uninitialized variables

| Scope | Default |
|-------|---------|
| File scope / `static` | zero-initialized (always) |
| `auto` (local) | **garbage** — undefined value, reading it is UB |

This is why `static int x;` is 0 but `int x;` inside a function is whatever was on the stack.

## `inline` functions (C99)

```c
static inline int max(int a, int b) {
    return a > b ? a : b;
}
```

- **hint** to the compiler: "consider inserting the function body at the call site instead of generating a function call"
- the compiler is free to ignore the hint — and usually knows better than you
- without `static`, `inline` has complex linkage rules. **Always use `static inline`** in headers.

### `static inline` — the only pattern you need

```c
// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }
static inline int clamp(int x, int lo, int hi) {
    return x < lo ? lo : x > hi ? hi : x;
}

#endif
```

- `static` = each translation unit gets its own copy (avoids duplicate symbol errors)
- `inline` = hint to inline the body (avoids function call overhead for tiny functions)
- this is how you put "small functions" in headers without linker errors

### Why not macros?

`static inline` is safer than `#define` macros:

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int x = MAX(i++, j++);     // bug: i++ or j++ evaluated twice

static inline int max(int a, int b) { return a > b ? a : b; }
int x = max(i++, j++);     // correct: each argument evaluated once
```

Macros still win for type-generic operations (before C11 `_Generic`), but for same-type helpers, prefer `static inline`.

## Idioms

- **one declaration per line** — avoids `int *p, q` ambiguity
- **initialize at declaration** — `int x = 0;` not `int x; ... x = 0;`
- **`static` by default** — only expose what you need to. Start private, widen scope only when required.
- **`const` everything you can** — function parameters, pointers to read-only data, file-scope constants
- **`volatile` for hardware and ISR variables** — if a variable can change outside normal program flow, it's `volatile`
- **`static inline` for small helpers in headers** — not macros
- **`typedef` to tame complex declarations** — if you can't read the declaration in 3 seconds, typedef the intermediate types

---

## Hardware / low-level idioms

### Hardware register map (peripheral struct)

**What:** A `volatile` struct whose members correspond to hardware registers at consecutive memory addresses, accessed through a macro-cast pointer.
**Why:** Type-safe, named access to memory-mapped I/O registers instead of raw pointer arithmetic. The struct layout mirrors the hardware — field order and size must match the reference manual exactly.
**When:** Any time you write bare-metal code that talks directly to peripherals.
**Where:** Every MCU vendor header — STM32 CMSIS (`stm32f401xe.h`), ESP-IDF, Pico SDK, Linux kernel (`__iomem`).

```c
// define the register layout — matches RM0401 section 8.4 (GPIO registers)
typedef struct {
    volatile uint32_t MODER;     // offset 0x00 — mode register
    volatile uint32_t OTYPER;    // offset 0x04 — output type
    volatile uint32_t OSPEEDR;   // offset 0x08 — output speed
    volatile uint32_t PUPDR;     // offset 0x0C — pull-up/pull-down
    volatile uint32_t IDR;       // offset 0x10 — input data (read-only)
    volatile uint32_t ODR;       // offset 0x14 — output data
    volatile uint32_t BSRR;      // offset 0x18 — bit set/reset
    volatile uint32_t LCKR;      // offset 0x1C — lock
    volatile uint32_t AFR[2];    // offset 0x20–0x24 — alternate function
} GPIO_TypeDef;

// map to the physical base address from the datasheet
#define GPIOA ((GPIO_TypeDef *)0x40020000)
#define GPIOB ((GPIO_TypeDef *)0x40020400)
#define GPIOC ((GPIO_TypeDef *)0x40020800)

// usage — clean, readable, type-checked:
GPIOA->MODER |= (1U << 10);      // set PA5 to output mode
GPIOA->ODR   ^= (1U << 5);       // toggle PA5
uint32_t btn = GPIOC->IDR & (1U << 13);  // read PC13 (user button)
```

Why every field is `volatile`: the hardware can change `IDR` at any time (a button press), and your writes to `ODR` must actually reach the hardware — the compiler must not cache, reorder, or eliminate any access.

### Bit manipulation macros

**What:** Macros for setting, clearing, toggling, and reading individual bits in hardware registers.
**Why:** Raw bitwise expressions like `reg |= (1U << 5)` are error-prone and hard to read. Named macros make register operations self-documenting.
**When:** Any bare-metal register manipulation. Used on every line of low-level embedded code.
**Where:** STM32 HAL (`SET_BIT`, `CLEAR_BIT`, `MODIFY_REG`), Linux kernel (`BIT()`, `GENMASK()`), every vendor SDK.

```c
#define BIT(n)                 (1U << (n))
#define SET_BIT(reg, mask)     ((reg) |= (mask))
#define CLEAR_BIT(reg, mask)   ((reg) &= ~(mask))
#define TOGGLE_BIT(reg, mask)  ((reg) ^= (mask))
#define READ_BIT(reg, mask)    ((reg) & (mask))

// multi-bit field: clear old value, set new one
#define MODIFY_FIELD(reg, mask, val) \
    ((reg) = ((reg) & ~(mask)) | (val))

// usage:
SET_BIT(GPIOA->MODER, BIT(10));            // set bit 10
CLEAR_BIT(GPIOA->MODER, BIT(11));          // clear bit 11
if (READ_BIT(GPIOC->IDR, BIT(13))) { }     // test bit 13

// set PA5 to alternate function mode (bits 11:10 = 0b10):
MODIFY_FIELD(GPIOA->MODER, 0x3U << 10, 0x2U << 10);
```

STM32 HAL also defines `MODIFY_REG(reg, clearmask, setmask)` — same idea. Once you recognize the pattern, vendor code becomes readable.

### ISR-safe volatile flag

**What:** A `volatile` variable shared between an interrupt service routine (ISR) and the main loop, used to signal that an event happened.
**Why:** Without `volatile`, the compiler sees the main loop never modifies the flag — it "optimizes" the check into an infinite loop. The ISR modifies it asynchronously, outside the compiler's view.
**When:** Any embedded program that uses interrupts to signal events (button press, data received, timer expired).
**Where:** Every bare-metal embedded system. FreeRTOS replaces this with task notifications/queues, but the pattern persists in simple firmware.

```c
// shared flag — must be volatile
static volatile int uart_rx_ready = 0;
static volatile uint8_t uart_rx_byte;

// ISR — runs asynchronously when UART receives a byte
void USART2_IRQHandler(void) {
    if (READ_BIT(USART2->SR, BIT(5))) {   // RXNE flag
        uart_rx_byte = (uint8_t)USART2->DR;
        uart_rx_ready = 1;
    }
}

// main loop — polls the flag
int main(void) {
    uart_init();
    while (1) {
        if (uart_rx_ready) {
            uart_rx_ready = 0;             // clear flag (order matters)
            process_byte(uart_rx_byte);
        }
        // do other work...
    }
}
```

**Gotchas:**
- clear the flag *before* processing, not after — an interrupt can fire during processing
- for multi-byte data, the flag must be set *after* all data bytes are written (acts as a release fence on most MCU architectures)
- `volatile` alone is not enough for multi-core or complex shared state — use atomics or mutexes there. On single-core MCUs with single-byte flags, `volatile` is sufficient.

### `_Static_assert` for hardware assumptions (C11)

**What:** Compile-time assertion that verifies a condition. If it fails, compilation stops with your message.
**Why:** Catch incorrect assumptions about struct sizes, alignment, and type widths before the code runs on hardware — where bugs are 100x harder to find.
**When:** Anywhere you have a hard requirement: struct must match a hardware register layout, a buffer must be a certain size, a type must be a certain width.
**Where:** Embedded firmware, protocol implementations, kernel code, any code that makes assumptions about memory layout.

```c
#include <stdint.h>
#include <stddef.h>

// verify the GPIO struct matches hardware — 10 registers × 4 bytes = 40 bytes
_Static_assert(sizeof(GPIO_TypeDef) == 40,
    "GPIO_TypeDef size mismatch — check register layout against reference manual");

// verify types are the width you expect
_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
_Static_assert(sizeof(void *) == 4,   "expected 32-bit pointers on this target");

// verify struct field offsets match hardware register offsets
_Static_assert(offsetof(GPIO_TypeDef, IDR) == 0x10,
    "IDR must be at offset 0x10 to match hardware");
_Static_assert(offsetof(GPIO_TypeDef, ODR) == 0x14,
    "ODR must be at offset 0x14 to match hardware");

// verify a buffer fits in a hardware constraint
#define DMA_BUF_SIZE 256
_Static_assert(DMA_BUF_SIZE <= 65535, "DMA buffer exceeds 16-bit transfer count");
_Static_assert((DMA_BUF_SIZE & (DMA_BUF_SIZE - 1)) == 0,
    "DMA buffer size must be a power of 2");
```

If you change the struct and accidentally break the layout, the compiler tells you immediately — not a mysterious crash on the board at 2am.

### Callback with context (function pointer + `void *`)

**What:** A function pointer paired with a `void *` user data pointer — C's idiomatic closure.
**Why:** The callback needs to access caller-specific state, but the registering API doesn't know the caller's types. `void *` carries the context opaquely.
**When:** Event handlers, interrupt callbacks, timer callbacks, any registration API where the callee is generic but the callback needs caller-specific data.
**Where:** FreeRTOS timer callbacks, ESP-IDF event loop, GTK signal handlers, `pthread_create`, `qsort_r`, embedded HAL callback registration.

```c
// generic timer API — doesn't know what the callback needs
typedef void (*TimerCallback)(void *ctx);

typedef struct {
    uint32_t interval_ms;
    TimerCallback callback;
    void *user_ctx;            // opaque pointer — timer doesn't touch it
} SoftTimer;

void timer_register(SoftTimer *t);

// caller's specific data
typedef struct {
    int pin;
    int count;
} BlinkState;

// callback receives its context back
void blink_tick(void *ctx) {
    BlinkState *s = (BlinkState *)ctx;
    toggle_pin(s->pin);
    s->count++;
}

// usage:
BlinkState state = {.pin = 5, .count = 0};
SoftTimer t = {
    .interval_ms = 500,
    .callback = blink_tick,
    .user_ctx = &state,          // pass context — timer stores it, callback receives it
};
timer_register(&t);
```

The `void *` is the glue — the timer stores it blindly, the callback casts it back. This is how C achieves what closures/lambdas do in higher-level languages. You'll see this pattern in every embedded SDK.
