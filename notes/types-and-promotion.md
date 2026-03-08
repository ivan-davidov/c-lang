# Types and Integer Promotion

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
