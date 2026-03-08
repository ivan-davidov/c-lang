# Undefined Behavior and Evaluation Order

- **precedence** determines grouping (`a + b * c`), it's fixed
- **evaluation order** of operands is unspecified — compiler picks any order
  ```c
  f() + g()    // f() or g() may run first — don't care which? fine
  a[i] = i++;  // UB — both read and modify i between sequence points
  ```
- **sequence points** guarantee completion of side effects before continuing:
  `;`  `&&`  `||`  `?:`  `,` (comma operator)  function call (after args evaluated)
- between two sequence points, modifying a variable more than once is UB
- `&&` and `||` short-circuit and evaluate left-to-right (guaranteed):
  ```c
  if (p != NULL && *p > 0)  // safe — *p only reached if p is valid
  ```
- compiler exploits UB for optimization — code "works in debug, breaks in release"
  is often UB that the optimizer exposed

## Defensive idioms

- null guard with short-circuit:
  ```c
  if (p != NULL && p->value > 0)    // safe — second part skipped if p is NULL
  if (n > 0 && arr[n - 1] == x)     // safe — no out-of-bounds if n is 0
  ```

- `assert()` for invariants during development:
  ```c
  #include <assert.h>
  assert(index < array_size);       // crashes with file:line in debug
  // compiles to nothing with -DNDEBUG (release builds)
  ```

- initialize everything:
  ```c
  int x = 0;                        // not "int x;" — never leave uninitialized
  char buf[256] = {0};              // zero-fill entire array
  struct Foo f = {0};               // zero-init all struct fields
  ```

- check return values — C has no exceptions:
  ```c
  FILE *fp = fopen(path, "r");
  if (fp == NULL) { perror("fopen"); return -1; }
  ```

- avoid double evaluation in macros (why `do { } while(0)` and `((void)0)` exist):
  ```c
  // BAD:  #define MAX(a,b) ((a) > (b) ? (a) : (b))  — evaluates a or b twice
  // OK for simple args, dangerous if args have side effects: MAX(i++, j++)
  ```
