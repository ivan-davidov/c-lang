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
| `bool`      | `<stdbool.h>`  | `true`/`false` — any nonzero converts to 1 (C99; keyword in C23) |
| `NULL`      | `<stddef.h>`   | null pointer constant — `(void *)0`    |

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
