# Functions

## Basics

- every function has: return type, name, parameter list, body
- `void` return type = no return value, `void` parameter = no parameters
- C is **pass-by-value** — always. Function gets a copy. To modify the original, pass a pointer.
  ```c
  void double_it(int x)   { x *= 2; }           // useless — modifies local copy
  void double_it(int *x)  { *x *= 2; }          // works — modifies through pointer
  ```
- arrays are the exception that proves the rule — they decay to a pointer, so the function operates on the original data (but the pointer itself is still a copy)

## Declaration vs definition

```c
// declaration (prototype) — tells compiler the signature, no body
int add(int a, int b);

// definition — the actual implementation
int add(int a, int b) {
    return a + b;
}
```

- declarations go in header files (`.h`), definitions in source files (`.c`)
- without a declaration, calling a function before its definition is an error in C99+
- old C (K&R style) allowed implicit declaration — calling an unknown function assumed it returned `int`. This is gone in C99. Don't rely on it.

## Parameter quirks

- array parameters decay to pointers — these are identical:
  ```c
  void f(int arr[10]);    // the 10 is ignored — it's just int *
  void f(int arr[]);      // same — still int *
  void f(int *arr);       // what the compiler actually sees
  ```
  always pass length separately: `void f(int *arr, size_t len)`

- `const` on parameters documents intent and enables compiler checks:
  ```c
  void print_arr(const int *arr, size_t len);  // promises not to modify arr
  ```

- C has no function overloading — one name, one signature. Use different names or `_Generic` (C11, rarely used).

## Return values

- returning a local variable's value is fine — it's copied out
- returning a pointer to a local variable is UB — the local is gone after return:
  ```c
  int *bad(void) {
      int x = 42;
      return &x;        // dangling pointer — x is dead after return
  }
  ```
- return `0` from `main` = success, nonzero = failure (convention, not enforced)
- `EXIT_SUCCESS` and `EXIT_FAILURE` from `<stdlib.h>` — more portable than `0` and `1`

## `static` functions

```c
static void helper(void) { /* ... */ }
```

- visible only within the file — internal linkage
- the C equivalent of "private"
- use liberally — if a function isn't needed outside the file, make it `static`
- also prevents name collisions across files

## `inline` functions

```c
inline int max(int a, int b) { return a > b ? a : b; }
```

- hint to the compiler to substitute the function body at the call site (no function call overhead)
- compiler is free to ignore it — modern compilers inline automatically when it helps
- usually defined in headers since the body must be visible at each call site
- rarely needed — trust the compiler's optimizer with `-O2`

## Variadic functions (`...`)

```c
#include <stdarg.h>
int sum(int count, ...) {
    va_list args;
    va_start(args, count);
    int total = 0;
    for (int i = 0; i < count; i++)
        total += va_arg(args, int);
    va_end(args);
    return total;
}
sum(3, 10, 20, 30);  // → 60
```

- `printf` is a variadic function — that's how it takes any number of args
- no type safety — passing wrong type is UB. This is why `printf` format mismatches are dangerous.
- avoid writing your own unless you have a good reason

## Function pointers

```c
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }

int (*op)(int, int);        // declare pointer to function taking 2 ints, returning int
op = add;                   // point to add (& is optional: op = &add)
int result = op(3, 2);      // call through pointer → 5
op = sub;
result = op(3, 2);          // → 1
```

- typedef makes them readable:
  ```c
  typedef int (*BinOp)(int, int);
  BinOp op = add;
  ```

- main use cases:
  - **callbacks**: `qsort(arr, n, sizeof(int), compare_fn)`
  - **state machines**: transition table of function pointers
  - **dispatch tables**: replace long switch/if chains
  ```c
  BinOp ops[] = {add, sub, mul, div_fn};
  result = ops[operator_index](a, b);     // dispatch by index
  ```

## Recursion

- function calls itself — needs a base case to stop
- every call gets its own stack frame (local variables, return address)
- deep recursion = stack overflow, especially on embedded (1-4 KB stack)
- tail recursion optimization: if the recursive call is the last thing in the function, compiler *may* reuse the stack frame — but C doesn't guarantee it
- iterative solutions are generally preferred in C — explicit loops, no stack risk

## Idioms

- **error return pattern** — return `int` where `0` = success, negative = error:
  ```c
  int parse_config(const char *path, Config *out) {
      FILE *fp = fopen(path, "r");
      if (!fp) return -1;
      // ... parse into *out ...
      fclose(fp);
      return 0;
  }

  if (parse_config("app.cfg", &cfg) != 0) {
      fprintf(stderr, "failed to load config\n");
  }
  ```

- **output parameters** — return multiple values via pointers:
  ```c
  void divmod(int a, int b, int *quotient, int *remainder) {
      *quotient = a / b;
      *remainder = a % b;
  }
  int q, r;
  divmod(17, 5, &q, &r);    // q = 3, r = 2
  ```

- **init/destroy pairs** — resource management:
  ```c
  Context *ctx_create(void);
  void ctx_destroy(Context *ctx);
  ```

- **`_Generic`** (C11) — poor man's overloading:
  ```c
  #define abs_val(x) _Generic((x), \
      int:    abs,                  \
      float:  fabsf,               \
      double: fabs                  \
  )(x)
  ```
  rarely seen in practice but good to know it exists
