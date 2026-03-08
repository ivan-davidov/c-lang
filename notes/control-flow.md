# Control Flow

- **dangling else** — `else` binds to the nearest unmatched `if`:
  ```c
  if (a)
      if (b)
          x = 1;
  else          // this belongs to if (b), not if (a)
      x = 2;
  ```
  always use braces to avoid ambiguity

- **switch fallthrough** is silent and intentional in C — no warning by default:
  ```c
  switch (x) {
      case 1: do_a();    // falls into case 2
      case 2: do_b();    // runs for both x==1 and x==2
              break;
  }
  ```
  every `case` needs explicit `break` unless you want fallthrough

- `switch` only works on integer types — no strings, no floats

- **conditional operator** `a ? b : c` — the only ternary, useful for
  assignments and return values, don't nest it

- `=` vs `==` in conditions — `if (x = 5)` assigns and is always true,
  compiles without error, classic C bug. some people write `if (5 == x)`
  to catch it (yoda conditions) — or just use `-Wall`

## Common idioms

- **early return** to reduce nesting:
  ```c
  // instead of deeply nested if/else:
  int process(int *p, int n) {
      if (p == NULL) return -1;
      if (n <= 0) return -1;
      // happy path here, no nesting
  }
  ```

- **ternary for min/max/clamp**:
  ```c
  int min = (a < b) ? a : b;
  int max = (a > b) ? a : b;
  int clamped = (x < lo) ? lo : (x > hi) ? hi : x;
  ```

- **switch on enum** for exhaustive handling:
  ```c
  typedef enum { STATE_IDLE, STATE_RUN, STATE_ERR } State;
  switch (state) {
      case STATE_IDLE: /* ... */ break;
      case STATE_RUN:  /* ... */ break;
      case STATE_ERR:  /* ... */ break;
      // no default — compiler warns if you add a new enum value and forget a case (-Wswitch)
  }
  ```

- **intentional fallthrough** — document it:
  ```c
  switch (level) {
      case 3: do_extra();    /* fallthrough */
      case 2: do_more();     /* fallthrough */
      case 1: do_base();     break;
  }
  ```

- **flag-based dispatch** over long if/else chains:
  ```c
  if (flags & FLAG_VERBOSE) print_details();
  if (flags & FLAG_DRY_RUN) return 0;
  ```
