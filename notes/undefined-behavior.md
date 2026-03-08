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
