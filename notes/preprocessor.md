# The Preprocessor

Runs before compilation. Pure text substitution — no types, no scope, no syntax awareness.

## `#include`

```c
#include <stdio.h>       // search system include paths
#include "myheader.h"    // search current directory first, then system paths
```

Literally pastes the file contents. That's why include guards exist — without them, including the same header twice duplicates everything.

## `#define` — object-like macros

```c
#define MAX_SIZE 1024
#define PI 3.14159
```

- no `=`, no `;` — those become part of the substitution
- convention: `ALL_CAPS` for macros to distinguish from variables
- `#undef MAX_SIZE` — undefine a macro

## `#define` — function-like macros

```c
#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
```

### Parenthesize everything

```c
// BAD:
#define SQUARE(x) x * x
SQUARE(1 + 2)     // expands to: 1 + 2 * 1 + 2 = 5, not 9

// GOOD:
#define SQUARE(x) ((x) * (x))
SQUARE(1 + 2)     // expands to: ((1 + 2) * (1 + 2)) = 9
```

### Double evaluation trap

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
MAX(i++, j++)     // one of them gets incremented TWICE
```

No fix in pure macros. This is why `static inline` functions are preferred for anything with side effects:
```c
static inline int max(int a, int b) { return a > b ? a : b; }
```

### Multi-statement macros — `do { } while(0)`

```c
// BAD:
#define LOG(msg) printf("[LOG] "); printf("%s\n", msg);
if (error)
    LOG("fail");    // only first printf is inside the if

// GOOD:
#define LOG(msg) do { printf("[LOG] "); printf("%s\n", msg); } while(0)
if (error)
    LOG("fail");    // whole block is one statement, works with if/else
```

## `#` and `##` operators

```c
// # — stringification: turns argument into a string literal
#define STR(x) #x
STR(hello)          // → "hello"
STR(1 + 2)          // → "1 + 2"

// ## — token pasting: glues two tokens together
#define CONCAT(a, b) a##b
CONCAT(my, var)     // → myvar
CONCAT(uint, 32_t)  // → uint32_t
```

Used in real code for:
```c
// debug printing with variable name
#define DEBUG(var) printf(#var " = %d\n", var)
DEBUG(count);       // → printf("count" " = %d\n", count); → prints "count = 42"

// generate function names
#define TEST(name) void test_##name(void)
TEST(parser) { /* ... */ }    // → void test_parser(void) { ... }
```

## Conditional compilation

```c
#ifdef DEBUG
    printf("debug: x = %d\n", x);
#endif

#ifndef CONFIG_H       // include guard pattern
#define CONFIG_H
// ...
#endif

#if LEVEL > 3          // arithmetic on macros
    // ...
#elif LEVEL > 1
    // ...
#else
    // ...
#endif

#if defined(LINUX) && !defined(ARM)
    // platform-specific code
#endif
```

### Common uses

- **debug logging:**
  ```c
  #ifdef DEBUG
  #define DBG(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
  #else
  #define DBG(fmt, ...)    // compiles to nothing
  #endif
  ```
  `##__VA_ARGS__` — the `##` removes the trailing comma when no args (gcc/clang extension)

- **platform-specific code:**
  ```c
  #if defined(_WIN32)
      // windows
  #elif defined(__linux__)
      // linux
  #elif defined(__APPLE__)
      // macos
  #endif
  ```

- **feature toggles:**
  ```c
  #define USE_MQTT 1
  #if USE_MQTT
      mqtt_publish(data);
  #endif
  ```

## Predefined macros

| Macro | Value |
|-------|-------|
| `__FILE__` | current filename as string literal |
| `__LINE__` | current line number as integer |
| `__func__` | current function name (C99, technically not preprocessor) |
| `__DATE__` | compilation date |
| `__TIME__` | compilation time |
| `__STDC__` | 1 if standard-conforming compiler |
| `__STDC_VERSION__` | `201710L` for C17, `201112L` for C11 |

```c
#define ASSERT(cond) \
    do { if (!(cond)) { \
        fprintf(stderr, "ASSERT FAILED: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
        abort(); \
    }} while(0)
```

## Variadic macros (C99)

```c
#define LOG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
LOG("x=%d y=%d\n", x, y);    // → fprintf(stderr, "x=%d y=%d\n", x, y)
```

Problem: `LOG("hello\n")` fails — no args after fmt, trailing comma.
Fix (gcc/clang): `##__VA_ARGS__` eats the comma.
Fix (C23): `__VA_OPT__(,)` — standard way:
```c
#define LOG(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
```

## When to use macros vs alternatives

| Need | Macro | Alternative | Prefer |
|------|-------|-------------|--------|
| Constants | `#define N 100` | `enum { N = 100 };` or `static const int N = 100;` | `enum` for ints, `const` for others |
| Small functions | `#define MAX(a,b)` | `static inline int max(int a, int b)` | `inline` — type-safe, no double eval |
| Type-generic code | `#define SWAP(a,b)` | `_Generic` (C11) | macro if `_Generic` is too ugly |
| Debug/assert | `#define ASSERT(x)` | — | macro — needs `__FILE__`, `__LINE__`, `#x` |
| Conditional compilation | `#ifdef` | — | macro — only option |
| Include guards | `#ifndef` | `#pragma once` | either — `#pragma once` is simpler |
| String/token tricks | `#`, `##` | — | macro — only option |
