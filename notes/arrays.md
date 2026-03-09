# Arrays

## Basics

- fixed size, known at compile time (unless VLA)
- zero-indexed: `arr[0]` to `arr[n-1]`
- no bounds checking — accessing `arr[n]` is UB, not an error
- array name decays to pointer to first element in most contexts (see pointers notes later)
- size must be known: `int arr[10];` — stack allocated, not resizable

## Initialization

```c
int a[5] = {1, 2, 3, 4, 5};       // fully initialized
int b[5] = {1, 2};                  // b = {1, 2, 0, 0, 0} — rest zero-filled
int c[5] = {0};                     // all zeros — the idiom
int d[] = {1, 2, 3};                // compiler infers size 3
```

- `= {0}` zeros the entire array — use this, don't memset for simple cases
- designated initializers (C99):
  ```c
  int e[10] = {[3] = 42, [7] = 99};  // everything else is 0
  ```

## Memory layout

Arrays are contiguous bytes. Always. A 2D array is just a flat block with row-major indexing:

```
int m[2][3] = {{1,2,3}, {4,5,6}};

memory layout (row-major):
[1][2][3][4][5][6]
 ─────── ───────
  row 0   row 1

m[row][col] = *(m + row * 3 + col)   // what the compiler does
```

- **row-major** — rows are contiguous. Last index varies fastest.
- C, C++, Python (numpy default), Rust = row-major
- Fortran, MATLAB = column-major (opposite)

This matters for performance — iterating row-by-row is cache-friendly:
```c
// GOOD — sequential memory access (row-major order)
for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
        sum += m[i][j];

// BAD — jumping across rows (cache miss on every access)
for (int j = 0; j < cols; j++)
    for (int i = 0; i < rows; i++)
        sum += m[i][j];
```

## Multidimensional arrays vs array of pointers

### True 2D array — `int m[ROWS][COLS]`

```c
int m[3][4];    // single contiguous block: 3 * 4 * sizeof(int) bytes
```
- one allocation, cache-friendly, stack allocated
- both dimensions must be compile-time constants (unless VLA)
- passing to functions is ugly — must specify all dimensions except first:
  ```c
  void process(int m[][4], int rows);  // 4 is baked in
  ```

### Array of pointers — `int *rows[N]` or `int **m`

```c
int *m[3];                            // 3 pointers, each to a separate row
for (int i = 0; i < 3; i++)
    m[i] = malloc(cols * sizeof(int));
```
- each row is a separate allocation — not contiguous
- rows can have different lengths (ragged array)
- dimensions can be runtime values
- passing to functions is clean: `void process(int **m, int rows, int cols)`
- must free each row + the pointer array — more bookkeeping

### When to use which

| Use case | Preference |
|----------|-----------|
| Small, fixed-size (e.g. 3x3 matrix) | True 2D array — simplest |
| Performance-critical, large matrix | True 2D array — cache-friendly |
| Runtime dimensions, dynamic sizes | Array of pointers or flat 1D + index math |
| Ragged data (rows of different lengths) | Array of pointers — only option |
| Passing to many functions | Array of pointers — cleaner signatures |

### Flat 1D simulation — the embedded/performance approach

```c
int *m = malloc(rows * cols * sizeof *m);
m[row * cols + col] = value;          // manual index math
free(m);                               // single free
```
- one allocation, one free, contiguous, cache-friendly
- works with runtime dimensions
- index math is ugly but wrappable in a macro:
  ```c
  #define AT(m, cols, r, c) ((m)[(r) * (cols) + (c)])
  AT(m, cols, 2, 3) = 42;
  ```
- **this is what most serious C code uses** for dynamic 2D data — game engines, image processing, numerical code, embedded

## `const` arrays

- `const int arr[] = {1, 2, 3};` — elements can't be modified after initialization
- useful for lookup tables, configuration, read-only data:
  ```c
  static const char *const days[] = {   // const pointer to const strings
      "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
  };
  ```
- `static const` arrays in functions are allocated once, live for program lifetime — like a global but scoped
- compiler may place `const` arrays in read-only memory (`.rodata` section) — writing to them is UB and may crash (segfault)

## VLAs — variable-length arrays (C99)

```c
void process(int n) {
    int arr[n];    // size determined at runtime, allocated on stack
}
```

**Avoid them:**
- stack allocated with runtime size = stack overflow risk (especially embedded: 1-4 KB stack)
- optional in C11 — compiler can choose not to support them
- **Linux kernel bans them**
- **MISRA C bans them** (automotive/safety)
- `sizeof` evaluates at runtime for VLAs (exception to the normal rule)
- no way to check if allocation succeeded — just crashes

**Use instead:**
- fixed-size array if you know a reasonable upper bound
- `malloc` for runtime sizes — at least you can check for `NULL`

## Idioms

- zero an array:
  ```c
  int arr[100] = {0};                         // at declaration
  memset(arr, 0, sizeof arr);                  // after declaration
  ```

- array length macro:
  ```c
  #define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
  // only works on real arrays, NOT pointers — no warning if misused
  ```

- iterate with pointer (faster in some contexts, idiomatic in C):
  ```c
  int arr[] = {1, 2, 3, 4, 5};
  for (int *p = arr; p < arr + ARRAY_LEN(arr); p++)
      printf("%d\n", *p);
  ```

- pass array to function — always pass length:
  ```c
  void process(const int *arr, size_t len);   // pointer + length — the C way
  // inside: arr[i] works, but sizeof(arr) gives pointer size, not array size
  ```

- string is just `char[]` with null terminator:
  ```c
  char s[] = "hello";     // = {'h','e','l','l','o','\0'} — size 6
  char *p = "hello";      // pointer to string literal (read-only memory)
  ```
