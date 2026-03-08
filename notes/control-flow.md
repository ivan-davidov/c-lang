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
