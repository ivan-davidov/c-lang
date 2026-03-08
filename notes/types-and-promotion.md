# Types and Integer Promotion

## Fundamental sizes

| Type             | Guaranteed minimum | Typical (x86-64) | Notes                        |
|------------------|--------------------|-------------------|------------------------------|
| `char`           | 8 bits             | 8                 | signedness is impl-defined   |
| `short`          | 16 bits            | 16                |                              |
| `int`            | 16 bits            | 32                | "natural" word size          |
| `long`           | 32 bits            | 64 (Linux), 32 (Win) | platform trap             |
| `long long`      | 64 bits            | 64                |                              |
| `float`          | —                  | 32                | ~7 decimal digits precision  |
| `double`         | —                  | 64                | ~15 decimal digits precision |
| `void *`         | —                  | 64 (on 64-bit)    | pointer size = address width |

- C only guarantees *minimums* — `sizeof(int)` can differ across platforms
- `sizeof` returns bytes, result type is `size_t`
- relationship guaranteed: `sizeof(char)` == 1 <= `sizeof(short)` <= `sizeof(int)` <= `sizeof(long)` <= `sizeof(long long)`

## `sizeof`

- compile-time operator (not a function) — parentheses optional for variables: `sizeof x`
- parentheses required for types: `sizeof(int)`
- returns `size_t` (unsigned) — mixing in signed expressions hits the promotion trap:
  ```c
  int len = -1;
  if (len < sizeof(int))  // false! len promoted to huge unsigned value
  ```
- VLAs are the exception — `sizeof` evaluates at runtime for them

## Fixed-width types (`<stdint.h>`)

| Type       | Exact width | Use for                         |
|------------|-------------|---------------------------------|
| `int8_t`   | 8 bits      | signed byte                     |
| `uint8_t`  | 8 bits      | registers, raw bytes, buffers   |
| `int16_t`  | 16 bits     | signed half-word                |
| `uint16_t` | 16 bits     | hardware registers, protocol fields |
| `int32_t`  | 32 bits     | general signed                  |
| `uint32_t` | 32 bits     | memory addresses, register maps |
| `int64_t`  | 64 bits     | timestamps, large counters      |
| `uint64_t` | 64 bits     | file sizes, hashes              |

- **use these in embedded** — bare `int` changes size across architectures
- format specifiers: `PRIu32`, `PRIx16` etc. from `<inttypes.h>`:
  ```c
  uint32_t val = 0xDEADBEEF;
  printf("reg: %" PRIx32 "\n", val);
  ```

## Other important types from headers

| Type        | Header         | What it is                              |
|-------------|----------------|-----------------------------------------|
| `size_t`    | `<stddef.h>`   | unsigned, result of `sizeof`, array indexing, `malloc` arg |
| `ssize_t`   | `<sys/types.h>`| signed size — return type of `read()`/`write()` (POSIX, not C standard) |
| `ptrdiff_t` | `<stddef.h>`   | signed, result of pointer subtraction   |
| `intptr_t`  | `<stdint.h>`   | integer guaranteed to hold a pointer    |
| `bool`      | `<stdbool.h>`  | alias for `_Bool` (C99), becomes a keyword in C23 (no header needed) |
| `NULL`      | `<stddef.h>`   | null pointer constant — `(void *)0`    |

## `_Bool` / `bool`

- `_Bool` is a real unsigned integer type (C99) — stores only 0 or 1
- any nonzero value assigned to `_Bool` becomes 1, zero becomes 0:
  ```c
  _Bool b = 42;  // b == 1
  _Bool c = 0;   // c == 0
  ```
- `<stdbool.h>` provides `bool`, `true`, `false` as macros mapping to `_Bool`, `1`, `0`
- C23: `bool`, `true`, `false` become keywords — no include needed
- before C99 there was no boolean type — people used `int` with 0/1 or `#define TRUE 1`

## `typedef`

`typedef` creates an alias for an existing type — it doesn't create a new type.

```c
typedef unsigned long ulong;     // ulong is now shorthand for unsigned long
typedef int Seconds;             // Seconds is still just int — no type safety
```

### Why it exists

- **readability** — `Milliseconds timeout` is clearer than `uint32_t timeout`
- **portability** — change the underlying type in one place:
  ```c
  typedef uint32_t DeviceAddr;   // if hardware changes to 16-bit, change only here
  ```
- **hiding complexity** — function pointer types are unreadable without typedef:
  ```c
  // without typedef:
  void (*signal(int, void (*)(int)))(int);   // what?

  // with typedef:
  typedef void (*SignalHandler)(int);
  SignalHandler signal(int sig, SignalHandler handler);   // ah.
  ```

### `typedef` with structs — the C idiom

In C, `struct` is part of the type name. Without typedef you must write `struct` everywhere:
```c
struct Point {
    int x;
    int y;
};
struct Point p1;              // must say "struct Point" every time
```

Typedef lets you drop the `struct` keyword:
```c
typedef struct {
    int x;
    int y;
} Point;
Point p1;                     // clean — just "Point"
```

For self-referencing structs (linked list, tree), you need the tag name:
```c
typedef struct Node {
    int data;
    struct Node *next;        // can't say "Node *next" here — typedef isn't complete yet
} Node;
```
After this, `Node` and `struct Node` both work.

### Conventions

- **embedded / kernel** — typedef for hardware types: `typedef volatile uint32_t reg32_t;`
- **Linux kernel style** — avoids typedef for structs, writes `struct thing` explicitly (Linus's preference — "typedef hides what the type actually is")
- **most C projects** — typedef structs to avoid `struct` noise, especially in APIs
- **opaque types** — typedef hides the implementation:
  ```c
  // in header (public):
  typedef struct Context Context;          // forward declaration — user can't see inside
  Context *ctx_create(void);

  // in .c (private):
  struct Context { int internal_stuff; };  // only implementation knows the layout
  ```
  This is how you do encapsulation in C.

## Common type idioms

- array length:
  ```c
  int arr[10];
  size_t len = sizeof(arr) / sizeof(arr[0]);  // 10 — compile-time constant
  ```
  breaks when array decays to pointer (in function params) — always pass length separately

- limits — `<limits.h>` and `<stdint.h>`:
  ```c
  INT_MAX      // 2147483647 (on 32-bit int)
  INT_MIN      // -2147483648
  UINT_MAX     // 4294967295
  UINT8_MAX    // 255
  UINT16_MAX   // 65535
  UINT32_MAX   // 4294967295
  SIZE_MAX     // max value of size_t
  ```

- `malloc` returns `void*` — no cast needed in C (required in C++):
  ```c
  int *p = malloc(n * sizeof *p);   // sizeof *p = sizeof(int), stays correct if type changes
  ```
  `sizeof *p` instead of `sizeof(int)` — adapts automatically if you change the type of `p`

- safe unsigned comparison:
  ```c
  // instead of: if (len - 1 >= 0)   — always true for unsigned!
  // write:      if (len > 0)         — then use len - 1 inside
  ```

## Promotion and conversion

- uninitialized local variables hold garbage, not zero — reading them is UB
- `char` is *not* guaranteed signed or unsigned — it's implementation-defined
- `char`, `short` silently promote to `int` in any expression
- mixing signed + unsigned in a comparison promotes signed to unsigned:
  ```c
  unsigned x = 1;
  int y = -1;
  // (y < x) is FALSE — y becomes a huge unsigned value
  ```
  this will bite you with `uint8_t` register values on hardware
- integer division truncates toward zero: `7 / 2 == 3`, `-7 / 2 == -3`
- integer overflow on signed types is UB — compiler can optimize it away
- unsigned overflow wraps (defined) — this is why hardware registers use unsigned
- implicit narrowing: assigning `double` to `int` truncates silently, `int` to `char` may lose data — no warning without `-Wconversion`
- `const` doesn't mean "constant" — it means "read-only". Can't use `const int n = 5;` as array size in C (unlike C++)
