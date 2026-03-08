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

## Common C loop idioms

- read until EOF:
  ```c
  int c;
  while ((c = getchar()) != EOF)  // assignment in condition — idiomatic, not a bug
  ```
- read lines from file:
  ```c
  char buf[256];
  while (fgets(buf, sizeof(buf), fp) != NULL)
  ```
- walk a null-terminated string:
  ```c
  for (char *p = str; *p != '\0'; p++)  // or just: *p
  ```
- walk a linked list:
  ```c
  for (Node *n = head; n != NULL; n = n->next)
  ```
- iterate array with pointer instead of index:
  ```c
  for (int *p = arr; p < arr + len; p++)
  ```
- sentinel-controlled loop (array ends with special value):
  ```c
  int data[] = {3, 7, 1, -1};  // -1 is sentinel
  for (int i = 0; data[i] != -1; i++)
  ```
- counting down to zero (unsigned-safe):
  ```c
  for (size_t i = n; i-- > 0; )  // the "goes to" operator joke: i --> 0
  ```
  avoids the unsigned underflow trap of `for (size_t i = n-1; i >= 0; i--)` which never terminates
- poll/retry with timeout (embedded):
  ```c
  uint32_t start = millis();
  while (!(REG->STATUS & READY_BIT)) {
      if (millis() - start > TIMEOUT_MS) return -1;
  }
  ```
- double loop break with flag or goto:
  ```c
  int found = 0;
  for (int i = 0; i < rows && !found; i++)
      for (int j = 0; j < cols && !found; j++)
          if (matrix[i][j] == target) found = 1;
  ```

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
