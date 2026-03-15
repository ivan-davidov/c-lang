# Program Design

How to structure C programs into modules with clean APIs. Builds on `program-organization.md` (the mechanics) — this is the design layer.

## What is a module?

A module is a `.h` + `.c` pair that provides a coherent set of related functionality behind a defined interface. The header is the API, the `.c` is the implementation.

The key question: **what does the module hide?**

## Types of modules

### 1. Data pool

**What:** A header that just declares shared variables. No functions, no logic — just a bag of globals.

```c
// globals.h
extern int debug_level;
extern char config_path[256];
extern struct Options options;

// globals.c
int debug_level = 0;
char config_path[256] = "/etc/app.conf";
struct Options options = {0};
```

**Pros:** Simple, zero ceremony.
**Cons:** Everything is coupled to everything. Any file can read/write any variable. Testing is a nightmare. Avoid this pattern — it's the weakest form of module.

### 2. Library (stateless function collection)

**What:** A set of related functions with no internal state. Pure input-output.

```c
// mathutil.h
double clamp(double x, double lo, double hi);
double lerp(double a, double b, double t);
double map_range(double x, double in_lo, double in_hi, double out_lo, double out_hi);

// mathutil.c
#include "mathutil.h"

double clamp(double x, double lo, double hi) {
    return x < lo ? lo : x > hi ? hi : x;
}
// ...
```

**Pros:** No state, no side effects, easy to test, reusable.
**Cons:** Can't model things that have state (connections, devices, sessions).
**Where:** `<string.h>`, `<math.h>`, `<ctype.h>` — the C standard library is mostly this pattern.

### 3. Abstract object (singleton with hidden state)

**What:** A module that manages a single instance of some resource using file-scoped `static` variables. The state is hidden in the `.c` — the header exposes only functions.

```c
// logger.h
void logger_init(const char *path);
void logger_write(const char *msg);
void logger_close(void);

// logger.c
#include "logger.h"
#include <stdio.h>

static FILE *log_file = NULL;       // hidden state — only this file can touch it
static int msg_count = 0;

void logger_init(const char *path) {
    log_file = fopen(path, "a");
    msg_count = 0;
}

void logger_write(const char *msg) {
    if (!log_file) return;
    fprintf(log_file, "[%d] %s\n", ++msg_count, msg);
}

void logger_close(void) {
    if (log_file) fclose(log_file);
    log_file = NULL;
}
```

**Pros:** Clean API, state is fully encapsulated, caller can't corrupt internals.
**Cons:** Only ONE logger can exist. If you need two loggers (one for errors, one for debug), you're stuck. The singleton is baked into the module.
**Where:** Common in embedded — a single UART driver, a single SPI bus, a single sensor. Also `<stdio.h>` stdin/stdout/stderr are abstract objects.

### 4. Abstract Data Type (ADT — multiple instances)

**What:** A module that defines a type where the internal representation is hidden. Callers work with an opaque pointer — they can create multiple independent instances.

```c
// stack.h
typedef struct Stack Stack;          // incomplete type — users can't see inside

Stack *stack_create(int capacity);
void stack_destroy(Stack *s);
void stack_push(Stack *s, int value);
int stack_pop(Stack *s);
bool stack_empty(const Stack *s);
bool stack_full(const Stack *s);

// stack.c
#include "stack.h"
#include <stdlib.h>

struct Stack {                       // definition hidden in .c
    int *data;
    int top;
    int capacity;
};

Stack *stack_create(int capacity) {
    Stack *s = malloc(sizeof *s);
    if (!s) return NULL;
    s->data = malloc(capacity * sizeof *s->data);
    if (!s->data) { free(s); return NULL; }
    s->top = 0;
    s->capacity = capacity;
    return s;
}

void stack_destroy(Stack *s) {
    if (!s) return;
    free(s->data);
    free(s);
}

void stack_push(Stack *s, int value) {
    if (s->top < s->capacity)
        s->data[s->top++] = value;
}

int stack_pop(Stack *s) {
    return s->data[--s->top];
}

bool stack_empty(const Stack *s) { return s->top == 0; }
bool stack_full(const Stack *s)  { return s->top == s->capacity; }
```

```c
// usage — multiple independent stacks:
Stack *a = stack_create(100);
Stack *b = stack_create(50);
stack_push(a, 42);
stack_push(b, 99);
// a and b are independent
stack_destroy(a);
stack_destroy(b);
```

**Pros:** Multiple instances, fully encapsulated, implementation can change without breaking callers.
**Cons:** Heap allocation for every instance, function call overhead (no inlining since the struct definition is hidden), slight indirection cost.

### Comparison

| Type | State | Instances | Encapsulation | Use when |
|------|-------|-----------|---------------|----------|
| Data pool | exposed globals | N/A | none | almost never — legacy code |
| Library | none | N/A | N/A | stateless utilities |
| Abstract object | hidden (static) | 1 | good | singletons (HW peripherals) |
| ADT | hidden (opaque ptr) | many | best | general-purpose types |

## Information hiding

The only tool C gives you: **`static`** makes things invisible outside the translation unit.

```c
// stack.c

// PRIVATE — not in header, not callable from outside
static void grow_if_needed(Stack *s) {
    if (s->top >= s->capacity) {
        s->capacity *= 2;
        s->data = realloc(s->data, s->capacity * sizeof *s->data);
    }
}

// PUBLIC — declared in header
void stack_push(Stack *s, int value) {
    grow_if_needed(s);
    s->data[s->top++] = value;
}
```

What you can hide:
- **functions** — `static` functions in `.c` are private helper functions
- **variables** — `static` file-scope variables are private state
- **struct internals** — incomplete types (forward declaration in `.h`, definition only in `.c`)

What you CANNOT hide in C (unlike C++/Rust):
- there's no `private` keyword
- if a struct is defined in a header, anyone who includes it can access any field
- the only way to truly hide fields is the incomplete type / opaque pointer pattern

## Incomplete types

An incomplete type is a type whose size is unknown. You can declare pointers to it, but you can't create instances or access members.

```c
// in header — incomplete type
typedef struct Stack Stack;     // compiler knows Stack exists but not its size

Stack *s;                       // OK — pointer is always the same size
sizeof(Stack);                  // ERROR — size unknown
Stack s;                        // ERROR — can't allocate unknown size
s->top;                         // ERROR — members unknown
```

This is the mechanism behind opaque pointers. The struct definition in the `.c` file "completes" the type — but only for that translation unit.

### `FILE` — the canonical incomplete type in C

`FILE` is the textbook example of an ADT using an incomplete type:

```c
// what you see in <stdio.h>:
typedef struct _IO_FILE FILE;    // incomplete — you can't see inside

// what glibc actually defines (simplified, in internal headers):
struct _IO_FILE {
    int _flags;           // read/write/append mode flags
    char *_IO_read_ptr;   // current read position in buffer
    char *_IO_read_end;   // end of read buffer
    char *_IO_read_base;  // start of read buffer
    char *_IO_write_base; // start of write buffer
    char *_IO_write_ptr;  // current write position
    char *_IO_write_end;  // end of write buffer
    char *_IO_buf_base;   // start of backing buffer
    char *_IO_buf_end;    // end of backing buffer
    int _fileno;          // underlying file descriptor (fd)
    // ... many more fields: locks, wide char state, linked list of all FILEs
};
```

You never touch any of these fields. You only use `FILE *` through the API: `fopen`, `fclose`, `fprintf`, `fread`, `fgets`, etc.

Why it works as an ADT:
- **multiple instances** — you can have many `FILE *`s open simultaneously
- **hidden internals** — buffering strategy, fd management, locking are invisible
- **portable** — different platforms implement `FILE` completely differently (glibc vs musl vs MSVC), but your code using `fprintf` works on all of them
- **polymorphic** — the same `FILE *` API works for regular files, pipes, stdin/stdout, and even in-memory streams (`fmemopen`) — different backends, same interface

This is exactly the pattern you'll use in your Stack, Ring Buffer, and other ADTs.

## Stack ADT — implementation trade-offs

The same abstract interface, four different implementations:

### A. Fixed-size array (stack-allocated)

```c
#define STACK_SIZE 100

typedef struct {
    int data[STACK_SIZE];
    int top;
} Stack;
```

**Pro:** No malloc, no free, fastest, works on bare metal with no heap.
**Con:** Fixed size, size baked into the type, wastes memory if mostly empty.
**Best for:** Embedded with no heap, when max size is known at compile time.

### B. Dynamic array (heap-allocated data)

```c
typedef struct {
    int *data;        // heap-allocated
    int top;
    int capacity;
} Stack;
```

**Pro:** Grows as needed, efficient (amortized O(1) push with doubling).
**Con:** Requires heap, realloc can fail, struct is exposed if in header.
**Best for:** General-purpose, when size isn't known upfront.

### C. Abstract object (hidden singleton)

```c
// stack.h — no struct visible at all
void stack_init(int capacity);
void stack_push(int value);
int stack_pop(void);

// stack.c
static int *data;
static int top, capacity;
```

**Pro:** Fully hidden implementation, clean API, no pointers for callers to manage.
**Con:** Only ONE stack exists. Can't have two.
**Best for:** Single-purpose embedded drivers (one UART, one SPI bus).

### D. ADT with opaque pointer (the full solution)

```c
// stack.h
typedef struct Stack Stack;
Stack *stack_create(int capacity);
// ... (shown in full above)
```

**Pro:** Multiple instances, fully encapsulated, implementation can change freely.
**Con:** Every operation is a function call (can't inline), heap allocation per instance.
**Best for:** Libraries, reusable components, anything that needs multiple instances.

### Which to use?

```
Known fixed size + embedded? ──→ A (fixed array)
Need to grow? ──→ B (dynamic) or D (opaque)
Only one instance ever? ──→ C (abstract object)
Multiple instances + clean API? ──→ D (ADT)
```

## Naming conventions

C has no namespaces. Every public symbol shares one global namespace. Naming conventions are your only defense against collisions.

### Prefix everything public

```c
// BAD — generic names will collide with other code:
void init(void);
int count;
typedef struct Node Node;

// GOOD — module prefix:
void uart_init(void);
int uart_rx_count;
typedef struct UartNode UartNode;
```

### Common conventions

| Convention | Example | Used by |
|-----------|---------|---------|
| `module_action()` | `stack_push()`, `uart_init()` | Most C projects, POSIX |
| `ModuleAction()` | `StackPush()`, `UartInit()` | Windows API, some embedded |
| `module_type_t` | `stack_t`, `uart_config_t` | POSIX types (but `_t` is reserved by POSIX) |
| `Module_TypeName` | `Stack_Config` | Some embedded codebases |
| `ALL_CAPS` | `MAX_SIZE`, `UART_BAUD` | Macros only |
| `_leading_underscore` | `_internal` | **Reserved** — don't use (reserved for implementation) |

### Rules for names

- `_` or `__` prefix: **reserved by the C standard**. Never start a name with underscore at file scope, or double underscore anywhere.
- `_t` suffix: **reserved by POSIX**. `stack_t` might collide with a system type. Some projects use it anyway, some avoid it.
- macros: always `ALL_CAPS` — visual distinction from functions and variables
- `static` functions in `.c`: no prefix needed — they're private, collisions are impossible

### Embedded convention (STM32 style)

```c
// peripheral prefix + action
HAL_UART_Transmit()
HAL_GPIO_WritePin()
HAL_TIM_Start_IT()

// type names
GPIO_TypeDef
UART_HandleTypeDef
TIM_Base_InitTypeDef

// register/bit names
USART_SR_RXNE
GPIO_MODER_MODE5
RCC_AHB1ENR_GPIOAEN
```

You'll see this style in every STM32 HAL header. The verbose names look ugly but eliminate ambiguity in a codebase with 50 peripherals.

---

## Idioms

### Module with init/destroy lifecycle

**What:** A module that requires initialization before use and cleanup when done — `module_init()` / `module_deinit()` bracket all usage.
**Why:** Embedded peripherals need clock enable, pin config, register setup before first use, and graceful shutdown (disable interrupts, release DMA) on teardown.
**When:** Any module that manages a hardware resource or allocates memory internally.
**Where:** Every embedded HAL (`HAL_UART_Init`, `HAL_UART_DeInit`), ESP-IDF (`esp_wifi_init`, `esp_wifi_deinit`), databases (`sqlite3_open`, `sqlite3_close`).

```c
// uart.h
typedef struct {
    uint32_t baud_rate;
    uint8_t  word_length;
    uint8_t  stop_bits;
} UART_Config;

int  uart_init(const UART_Config *cfg);  // 0 = success
void uart_deinit(void);
int  uart_send(const uint8_t *data, size_t len);
int  uart_recv(uint8_t *buf, size_t len);

// uart.c
#include "uart.h"

static volatile bool initialized = false;

int uart_init(const UART_Config *cfg) {
    if (initialized) return -1;      // already initialized
    // enable clock, configure pins, set baud rate...
    initialized = true;
    return 0;
}

void uart_deinit(void) {
    if (!initialized) return;
    // disable interrupts, reset registers...
    initialized = false;
}
```

### Opaque handle for multiple instances

**What:** Combine incomplete type + create/destroy functions to give callers a handle they can't inspect.
**Why:** Decouples interface from implementation. You can change the struct internals, swap implementations, or add fields without recompiling any caller code.
**When:** Libraries, reusable data structures, any API that needs to support multiple independent instances.
**Where:** `FILE *` (stdio), `sqlite3 *` (SQLite), `SSL *` (OpenSSL), `TaskHandle_t` (FreeRTOS).

```c
// ringbuf.h — callers see only this
typedef struct RingBuf RingBuf;

RingBuf *rb_create(size_t capacity);
void     rb_destroy(RingBuf *rb);
int      rb_push(RingBuf *rb, uint8_t byte);
int      rb_pop(RingBuf *rb, uint8_t *out);
bool     rb_empty(const RingBuf *rb);

// ringbuf.c — implementation hidden
struct RingBuf {
    uint8_t *buf;
    size_t head, tail, capacity;
};
```

This is how your Stage 2 ring buffer project should be structured.

### Config struct pattern

**What:** Pass a struct of configuration options to `init()` instead of a long parameter list.
**Why:** Functions with 5+ parameters are unreadable and fragile (did you get the order right?). A config struct is self-documenting, order-independent with designated initializers, and new options can be added without changing existing callers (they zero-initialize to defaults).
**When:** Any initialization that has more than 2-3 parameters.
**Where:** ESP-IDF (`wifi_config_t`), STM32 HAL (`UART_InitTypeDef`), SDL (`SDL_WindowFlags`), Vulkan (`VkInstanceCreateInfo`).

```c
typedef struct {
    uint32_t baud;
    uint8_t  data_bits;      // default 8
    uint8_t  stop_bits;      // default 1
    bool     hw_flow_ctrl;   // default false
    size_t   rx_buf_size;    // default 256
} UART_Config;

// caller uses designated initializers — only sets what they care about:
UART_Config cfg = {
    .baud = 115200,
    .rx_buf_size = 1024,
    // everything else is 0/false — sensible defaults
};
uart_init(&cfg);
```
