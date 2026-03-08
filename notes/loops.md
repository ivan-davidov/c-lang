# Loops

## `for` vs `while` vs `do-while`

- `for` — when you know the iteration count or have init/update logic
- `while` — when looping on a condition with no natural counter
- `do-while` — guarantees at least one execution, rare but idiomatic for:
  ```c
  do {
      c = getchar();
  } while (c != '\n');
  ```
  and in macros (the `do { ... } while(0)` pattern — wraps multi-statement macros safely)

## Things that matter

- loop variable declared inside `for` is scoped to the loop (C99+):
  ```c
  for (int i = 0; i < n; i++)  // i doesn't exist after the loop
  ```
- comma operator in `for` — evaluates left to right, result is rightmost:
  ```c
  for (int i = 0, j = n; i < j; i++, j--)  // two variables, both update
  ```
- `break` exits the innermost loop only — no labeled breaks in C (unlike Java)
- `continue` skips to the next iteration — in `for`, the update expression still runs
- infinite loop idioms: `for (;;)` or `while (1)` — both common, pick one and be consistent
  - `for (;;)` is more common in embedded/kernel code

## Off-by-one

- the classic bug — fencepost problem: 10 fence posts, 9 gaps
- `<` vs `<=` in the condition is where it hides:
  ```c
  for (int i = 0; i < n; i++)   // runs n times: 0..n-1 (correct for arrays)
  for (int i = 0; i <= n; i++)  // runs n+1 times: 0..n (buffer overrun if indexing array[n])
  ```
- always think: does `i` represent an index (0-based) or a count (1-based)?

## `goto`

- not banned — used idiomatically in C for error cleanup:
  ```c
  if (open_file() < 0) goto cleanup;
  if (alloc_mem() < 0) goto cleanup_file;
  // ...
  cleanup_file:
      close_file();
  cleanup:
      return -1;
  ```
- Linux kernel uses this pattern everywhere — it's cleaner than deeply nested `if`s
- don't use it for anything else
