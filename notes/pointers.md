# Pointers and Arrays

## What a pointer is

A pointer is a variable that holds a memory address. That's it.

```c
int x = 42;
int *p = &x;     // p holds the address of x

// three things:
p                 // the address (e.g. 0x7ffd1234)
*p                // the value at that address (42) — dereference
&x                // "address of x" — same value as p
```

- `*` in a declaration means "pointer to": `int *p` — p is a pointer to int
- `*` in an expression means "dereference": `*p` — read/write what p points to
- `&` means "address of": `&x` — get the address of x
- a pointer has a type — it determines what `*p` gives you and how arithmetic works

## Pointer arithmetic

Arithmetic operates in units of the pointed-to type, not bytes:

```c
int arr[5] = {10, 20, 30, 40, 50};
int *p = arr;       // points to arr[0]

p + 1               // points to arr[1] — moved sizeof(int) bytes, not 1 byte
p + 3               // points to arr[3]
*(p + 2)            // value at arr[2] → 30
p2 - p1             // number of elements between two pointers (ptrdiff_t)
```

**Valid operations:**
- pointer + integer, pointer - integer (move forward/back)
- pointer - pointer (distance between, same array only)
- pointer comparison (`<`, `>`, `==`, `!=` — same array only)

**Invalid:**
- pointer + pointer (meaningless)
- pointer * anything, pointer / anything

## Arrays decay to pointers

When you use an array name in an expression, it decays to a pointer to its first element:

```c
int arr[5];
int *p = arr;        // arr decays to &arr[0]
```

**`arr` and `&arr[0]` are the same value but array decay is one-way** — `p` is a pointer variable (8 bytes, assignable), `arr` is not (it's the array itself).

Decay does NOT happen with:
- `sizeof(arr)` — gives full array size, not pointer size
- `&arr` — gives pointer to the whole array (`int (*)[5]`), not pointer to first element

## `a[i]` is `*(a + i)` — always

The subscript operator is syntactic sugar for pointer arithmetic:

```c
arr[3]    ≡  *(arr + 3)    ≡  *(3 + arr)    ≡  3[arr]   // yes, this compiles
```

This is why arrays are 0-indexed — `arr[0]` = `*(arr + 0)` = first element, zero offset.

## Passing arrays to functions

Arrays always decay to pointers when passed — the function never receives an array:

```c
void f(int arr[10]);     // the 10 is a lie — compiler ignores it
void f(int arr[]);       // same
void f(int *arr);        // what the compiler actually sees

// always pass length separately:
void f(int *arr, size_t len);
```

Inside the function, `sizeof(arr)` gives pointer size (8), not array size. This is why you must pass length.

## Pointer to pointer

```c
int x = 42;
int *p = &x;
int **pp = &p;

**pp       // 42 — dereference twice
*pp        // p — the pointer itself
pp         // address of p
```

Use cases:

- **function that modifies a pointer** — the caller needs to see the new address:
  ```c
  void grow(int **buf, size_t *cap) {
      *cap *= 2;
      *buf = realloc(*buf, *cap * sizeof **buf);
  }
  int *data = malloc(10 * sizeof *data);
  size_t cap = 10;
  grow(&data, &cap);   // data may now point somewhere else
  ```

- **array of strings** — `char **` is a pointer to an array of `char *`:
  ```c
  int main(int argc, char **argv) {
      for (int i = 0; i < argc; i++)
          printf("%s\n", argv[i]);     // argv[i] is char *
  }
  ```

- **function that allocates for the caller** — return via parameter:
  ```c
  int read_file(const char *path, char **out, size_t *len) {
      *out = malloc(file_size);
      // ... read into *out ...
      *len = file_size;
      return 0;                        // caller frees *out
  }
  char *data;
  size_t len;
  read_file("input.txt", &data, &len);
  free(data);
  ```

- **linked list operations** — pass `Node **head` so insert/delete can change the head:
  ```c
  void prepend(Node **head, int val) {
      Node *n = malloc(sizeof *n);
      n->data = val;
      n->next = *head;
      *head = n;                       // caller's head pointer updated
  }
  Node *list = NULL;
  prepend(&list, 42);                  // list now points to new node
  ```
  if you passed `Node *head`, the caller's `list` would still be `NULL`

- **array of pointers with dynamic rows** (ragged array):
  ```c
  char **lines = malloc(n * sizeof *lines);
  for (int i = 0; i < n; i++)
      lines[i] = strdup(some_string);  // each row independently allocated
  ```

## `const` and pointers

Read declarations right to left:

```c
const int *p;           // pointer to const int — can't modify *p, can reassign p
int const *p;           // same thing — const int and int const are identical
int *const p;           // const pointer to int — can modify *p, can't reassign p
const int *const p;     // const pointer to const int — can't modify anything
```

The rule: `const` applies to whatever is immediately to its left. If nothing is to its left, it applies to the right.

Idiomatic usage:
```c
void print(const char *s);           // "I won't modify your string"
void fill(int *const buf, int val);  // rare — "I won't point elsewhere" (usually pointless)
```

## `void *` — the generic pointer

```c
void *p;       // holds any address, no type information
int x = 42;
p = &x;        // OK — any pointer converts to void*
int *ip = p;   // OK in C — void* converts back (C++ requires cast)
```

- can't dereference `void *` — compiler doesn't know the size
- can't do arithmetic on `void *` (gcc allows it as extension, treating as 1 byte — non-standard)
- `malloc` returns `void *` — why it works with any type without casting
- used for generic functions: `memcpy(void *dst, const void *src, size_t n)`

## `NULL`

```c
int *p = NULL;       // pointer that points nowhere — (void *)0
if (p) { }           // false — NULL is falsy
if (!p) { }          // true — null check
if (p != NULL) { }   // explicit style — same thing
```

- dereferencing NULL is UB — usually segfault on modern OS, but not guaranteed
- always initialize pointers — `int *p = NULL;` not `int *p;`
- always check before dereference when pointer comes from outside (function params, malloc return)

## `restrict` (C99)

```c
void add(int *restrict a, int *restrict b, int *restrict out, size_t n) {
    for (size_t i = 0; i < n; i++)
        out[i] = a[i] + b[i];
}
```

- promise to the compiler: these pointers don't alias (don't point to overlapping memory)
- allows aggressive optimization — compiler can vectorize, reorder loads/stores
- if you lie (pointers do alias) — UB
- `memcpy` requires non-overlapping (`restrict`), `memmove` handles overlap

## Common patterns

- **iterate array with pointer:**
  ```c
  for (int *p = arr; p < arr + len; p++)
      process(*p);
  ```

- **swap via pointers:**
  ```c
  void swap(int *a, int *b) {
      int tmp = *a;
      *a = *b;
      *b = tmp;
  }
  ```

- **return allocated memory:**
  ```c
  int *make_array(size_t n) {
      int *p = malloc(n * sizeof *p);
      if (!p) return NULL;
      return p;                          // caller must free
  }
  ```

- **pointer to walk a string:**
  ```c
  size_t my_strlen(const char *s) {
      const char *p = s;
      while (*p) p++;                    // walk until null terminator
      return p - s;                      // pointer subtraction = length
  }
  ```

- **opaque pointer (handle):**
  ```c
  // header: user sees only the pointer, not the struct layout
  typedef struct Ctx Ctx;
  Ctx *ctx_new(void);
  void ctx_free(Ctx *c);

  // source: struct defined privately
  struct Ctx { int internal; };
  ```
