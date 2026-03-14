# Dynamic Memory

## Stack vs heap pointers

```c
int a = 42;              // stack — automatic, freed when function returns
int *p = &a;             // pointer to stack memory

int *q = malloc(sizeof(int));  // heap — manual, lives until you free() it
*q = 42;
free(q);                 // you decide when it dies
```

Both `p` and `q` are pointers — same size (8 bytes on 64-bit), same operations. The difference is **where the memory lives and who manages its lifetime**:

| | Stack | Heap |
|---|---|---|
| Allocated by | compiler (automatic) | you (`malloc`) |
| Freed by | compiler (function return) | you (`free`) |
| Size known at | compile time | runtime |
| Speed | very fast (move stack pointer) | slower (allocator bookkeeping) |
| Risk | stack overflow if too large | leaks if you forget to free |

## How `malloc` and `free` actually work

### What `malloc` does

`malloc(n)` asks the allocator for `n` bytes of contiguous heap memory. The allocator:

1. Searches its free list for a block >= n bytes
2. If none found, asks the OS for more memory (`sbrk` or `mmap`)
3. Returns a pointer to the usable region

But here's the key — the allocator stores **metadata** (at minimum: block size) just **before** the pointer it gives you:

```
what malloc returns
         |
         v
[header ][usable memory .........................]
 ^
 hidden metadata: size, flags, etc.
 you never see this — it lives at ptr - sizeof(header)
```

So when you get back `0x1000`, the actual allocation might start at `0x0FF0` with a 16-byte header containing the block size, allocation flags, and free-list pointers.

### How `free` knows the size

`free(ptr)` steps back from your pointer to read the hidden header:

```c
free(ptr);
// internally: header = (struct header *)(ptr - HEADER_SIZE);
// now it knows: this block is 400 bytes
// adds the block back to the free list
```

That's why:
- `free(NULL)` is safe (defined as no-op)
- `free(ptr + 5)` is UB — the header isn't there, allocator reads garbage
- `free(stack_ptr)` is UB — no header exists for stack memory
- double `free(ptr)` is UB — the header may already be overwritten by the free list

### What free does NOT do

- does NOT zero the memory — old data is still there (security risk)
- does NOT return memory to the OS (usually) — it goes back to the free list for reuse
- does NOT set your pointer to NULL — your pointer still holds the old address (dangling)

## `malloc` / `calloc` / `realloc` / `free`

```c
#include <stdlib.h>

// malloc — uninitialized memory
int *p = malloc(n * sizeof *p);       // n ints, contents are garbage
if (!p) { /* allocation failed */ }

// calloc — zero-initialized, overflow-safe multiplication
int *p = calloc(n, sizeof *p);        // n ints, all bits zero
// calloc(n, size) checks for n*size overflow internally — malloc(n*size) doesn't

// realloc — resize existing allocation
int *tmp = realloc(p, new_size * sizeof *p);
if (!tmp) { /* old p is still valid — don't lose it */ }
p = tmp;                               // only update after success

// free — release memory
free(p);
p = NULL;                              // prevent dangling pointer use
```

### The `realloc` trap

```c
// WRONG — if realloc fails, you've lost the original pointer:
p = realloc(p, new_size);   // if realloc returns NULL, old p is leaked

// RIGHT — use a temporary:
int *tmp = realloc(p, new_size);
if (!tmp) {
    free(p);      // or handle error while p is still valid
    return -1;
}
p = tmp;
```

## Common allocation patterns

### Allocate a single struct

```c
Node *n = malloc(sizeof *n);       // sizeof *n = sizeof(Node), adapts if type changes
if (!n) return NULL;
n->data = 42;
n->next = NULL;
```

Always `sizeof *ptr` not `sizeof(Type)` — stays correct if you change the type of `ptr`.

### Allocate an array

```c
int *arr = calloc(n, sizeof *arr);   // calloc for arrays — zero-init + overflow check
if (!arr) return NULL;
// use arr[0] .. arr[n-1]
free(arr);
```

### Allocate a string copy

```c
char *copy = malloc(strlen(s) + 1);    // +1 for '\0'
if (!copy) return NULL;
strcpy(copy, s);

// or just use strdup (POSIX, not standard C):
char *copy = strdup(s);               // malloc + strcpy in one call
```

### Allocate a 2D array (flat)

```c
int *matrix = calloc(rows * cols, sizeof *matrix);
matrix[row * cols + col] = value;      // manual indexing
free(matrix);                          // single free
```

### Grow a dynamic array

```c
typedef struct {
    int *data;
    size_t len;
    size_t cap;
} Vec;

int vec_push(Vec *v, int val) {
    if (v->len == v->cap) {
        size_t new_cap = v->cap ? v->cap * 2 : 8;    // double or start at 8
        int *tmp = realloc(v->data, new_cap * sizeof *tmp);
        if (!tmp) return -1;
        v->data = tmp;
        v->cap = new_cap;
    }
    v->data[v->len++] = val;
    return 0;
}

void vec_free(Vec *v) {
    free(v->data);
    *v = (Vec){0};       // zero everything — safe to reuse or ignore
}
```

This doubling strategy gives amortized O(1) push — same idea as `std::vector`, Python `list`, Rust `Vec`.

## Pointer to pointer — why and when

The core principle: C is pass-by-value. To modify a caller's pointer, you must pass a pointer **to** that pointer.

### Linked list head modification

The most important `**` idiom. Without it, you can't insert at the head:

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// WRONG — can't change caller's head:
void prepend(Node *head, int val) {
    Node *n = malloc(sizeof *n);
    n->data = val;
    n->next = head;
    head = n;              // only changes LOCAL copy — caller's head unchanged
}

// RIGHT — modify caller's head through pointer:
void prepend(Node **head, int val) {
    Node *n = malloc(sizeof *n);
    n->data = val;
    n->next = *head;
    *head = n;             // caller's head now points to new node
}

Node *list = NULL;
prepend(&list, 10);        // list is updated
prepend(&list, 20);        // list: 20 → 10 → NULL
```

### Elegant linked list delete with `**`

This is the Linus Torvalds "good taste" example — using `Node **` to eliminate special-casing the head:

```c
void delete(Node **head, int val) {
    Node **curr = head;                    // pointer to the "next" pointer
    while (*curr) {
        if ((*curr)->data == val) {
            Node *tmp = *curr;
            *curr = (*curr)->next;         // unlink — works for head AND middle
            free(tmp);
            return;
        }
        curr = &(*curr)->next;             // advance: now points to this node's "next" field
    }
}
```

No `if (node == head)` special case. `curr` always points to the pointer that needs updating — whether it's `head` itself or some node's `->next` field.

### Realloc wrapper

Caller's pointer may change — must pass `**`:

```c
int grow(int **buf, size_t *cap) {
    size_t new_cap = *cap * 2;
    int *tmp = realloc(*buf, new_cap * sizeof *tmp);
    if (!tmp) return -1;
    *buf = tmp;
    *cap = new_cap;
    return 0;
}

int *data = malloc(10 * sizeof *data);
size_t cap = 10;
grow(&data, &cap);     // data may now point to a different address
```

### Allocate-for-caller pattern

Function creates something, caller receives it via `**`:

```c
int read_file(const char *path, char **out, size_t *out_len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);

    char *buf = malloc(len + 1);
    if (!buf) { fclose(fp); return -1; }

    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    *out = buf;            // caller now owns this memory
    *out_len = len;
    return 0;
}

char *data;
size_t len;
if (read_file("input.txt", &data, &len) == 0) {
    // use data...
    free(data);            // caller's responsibility
}
```

### Array of strings

`char **` — array of pointers, each pointing to a separate string:

```c
char **split(const char *s, char delim, int *count) {
    // count delimiters, allocate array of char*, strdup each piece
    // caller frees each string AND the array:
    for (int i = 0; i < *count; i++) free(result[i]);
    free(result);
}
```

## Function pointers with dynamic dispatch

### Callback for generic operations

```c
// generic sort — works on any type via function pointer
void sort(void *base, size_t n, size_t size, int (*cmp)(const void *, const void *));

int cmp_int(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

int arr[] = {5, 2, 8, 1};
qsort(arr, 4, sizeof(int), cmp_int);     // stdlib qsort uses this pattern
```

### Destructor callback for generic containers

```c
typedef struct {
    void **items;
    size_t len;
    void (*free_item)(void *);    // caller provides cleanup function
} List;

void list_free(List *l) {
    for (size_t i = 0; i < l->len; i++)
        if (l->free_item) l->free_item(l->items[i]);
    free(l->items);
}

// for a list of strings:
List l = {.items = ..., .len = 5, .free_item = free};

// for a list of structs with nested allocations:
void user_free(void *p) {
    User *u = p;
    free(u->name);
    free(u);
}
List l = {.items = ..., .len = 5, .free_item = user_free};
```

### Event handler table

```c
typedef void (*Handler)(const Event *);

Handler handlers[EVENT_COUNT] = {
    [EVENT_CLICK]   = on_click,
    [EVENT_KEYDOWN] = on_keydown,
    [EVENT_RESIZE]  = on_resize,
};

void dispatch(const Event *e) {
    if (e->type < EVENT_COUNT && handlers[e->type])
        handlers[e->type](e);
}
```

## `restrict` — aliasing promise

### What it means

`restrict` tells the compiler: **this pointer is the only way to access this memory**. No other pointer aliases it. The compiler can then optimize aggressively — reorder loads, cache values in registers, vectorize loops.

```c
void add(int *restrict a, int *restrict b, int *restrict out, size_t n) {
    for (size_t i = 0; i < n; i++)
        out[i] = a[i] + b[i];
}
```

Without `restrict`, the compiler must assume `out` might overlap with `a` or `b`. Every write to `out[i]` could change what `a[i+1]` reads. So it can't vectorize or reorder — it must do one element at a time, reloading from memory each iteration.

With `restrict`, the compiler knows writes to `out` don't affect reads from `a` or `b`. It can load multiple elements at once, use SIMD instructions, and keep values in registers.

### Is it like Rust ownership?

Similar idea, completely different enforcement:

| | C `restrict` | Rust ownership |
|---|---|---|
| What | "I promise no aliasing" | "Compiler proves no aliasing" |
| Enforcement | none — if you lie, it's UB | compile-time — code won't build |
| Granularity | per-pointer, per-function | per-variable, lifetime-tracked |
| Consequence of violation | silent wrong results, impossible bugs | compiler error |

Rust's `&mut` is like an enforced `restrict` — only one mutable reference exists. C's `restrict` is the honor system version.

### What happens if you violate it

```c
int arr[10] = {1, 2, 3, ...};
add(arr, arr + 1, arr + 2, 5);   // VIOLATION — arr, arr+1, arr+2 overlap
// UB — compiler optimized assuming no overlap, results are silently wrong
// no crash, no warning, no error — just wrong output
// may work in debug, break in release (-O2) when optimizer kicks in
```

Not a compiler error. Not a runtime crash. Just silently produces wrong answers. The worst kind of bug.

### `memcpy` vs `memmove` — restrict in practice

```c
void *memcpy(void *restrict dst, const void *restrict src, size_t n);   // no overlap allowed
void *memmove(void *dst, const void *src, size_t n);                    // overlap OK
```

`memcpy` is faster because `restrict` lets it copy in any order (forward, backward, chunks). `memmove` must check for overlap direction and copy safely. Use `memcpy` when you know buffers don't overlap, `memmove` when they might.

### Should you care?

- **Day to day:** rarely write `restrict` yourself. The compiler is smart enough for most code.
- **Performance-critical loops:** yes — tight numerical code, DSP, image processing on embedded. `restrict` can make 2-4x difference.
- **Understanding libraries:** `memcpy` vs `memmove`, BLAS, FFT libraries all use `restrict`. Knowing what it means helps you avoid misusing them.
- **Embedded:** when writing DMA buffer handlers or zero-copy parsers, `restrict` communicates intent even if the optimizer doesn't need it.

## Flexible array member (C99)

### What it is

An array with no size as the **last** member of a struct. The actual size is determined at allocation time:

```c
typedef struct {
    size_t len;
    char data[];       // flexible array — no size, must be last
} Buffer;
```

`sizeof(Buffer)` gives you the size of the header only (the `len` field + padding). The `data[]` member has zero size in the struct layout.

### How to use it

```c
// allocate header + n bytes of data in ONE allocation
Buffer *buf = malloc(sizeof(Buffer) + n);
if (!buf) return NULL;
buf->len = n;
memcpy(buf->data, source, n);

// one pointer, one free — data is contiguous with header
free(buf);
```

### Why use it instead of `char *data`

| | Flexible array (`data[]`) | Pointer (`char *data`) |
|---|---|---|
| Allocations | 1 (`malloc` for header + data) | 2 (`malloc` header, `malloc` data) |
| Frees | 1 | 2 (must free data, then struct) |
| Cache | contiguous — header and data adjacent | pointer chase — data could be anywhere |
| Copy | one `memcpy` of the whole thing | deep copy needed |
| Sizeof | `sizeof(Buffer)` excludes data | `sizeof` includes pointer (8 bytes) |

### Real-world examples

Network packet:
```c
typedef struct {
    uint8_t type;
    uint16_t length;
    uint8_t payload[];         // variable-length payload follows header
} Packet;

Packet *pkt = malloc(sizeof(Packet) + payload_len);
pkt->type = MSG_DATA;
pkt->length = payload_len;
memcpy(pkt->payload, data, payload_len);
send(fd, pkt, sizeof(Packet) + payload_len, 0);
```

Dynamic string:
```c
typedef struct {
    size_t len;
    size_t cap;
    char data[];
} Str;

Str *str_new(const char *s) {
    size_t len = strlen(s);
    Str *str = malloc(sizeof(Str) + len + 1);
    str->len = len;
    str->cap = len + 1;
    memcpy(str->data, s, len + 1);    // include '\0'
    return str;
}
```

### The old "struct hack" (pre-C99)

Before C99, people wrote `char data[1]` and over-allocated:

```c
typedef struct {
    size_t len;
    char data[1];       // hack — allocate more than 1
} Buffer;

Buffer *b = malloc(sizeof(Buffer) + n - 1);   // -1 because data[1] already has 1 byte
```

This was technically UB but worked everywhere. C99 made it official with `data[]`. **Use `data[]`, not `data[1]`.**

### Constraints

- flexible array must be the **last** member
- struct must have at least one other member before it
- cannot be stack-allocated — must use `malloc`
- cannot be used as array element: `Buffer arr[10]` won't work (compiler doesn't know the size)
- cannot assign structs with flexible arrays: `b1 = b2` copies only the header

### Should you avoid incomplete types?

No — flexible array members are not incomplete types. The struct itself is complete; the array just has unspecified size. Incomplete types are forward-declared structs like `struct Foo;` without a body — those are essential for opaque pointers and you should use them freely.

## Memory bugs — the big four

### 1. Leak — forgot to free

```c
char *s = strdup("hello");
return;                        // s leaked — no one will free it
```

Detection: Valgrind (`--leak-check=full`), ASan (`-fsanitize=address`).

### 2. Use after free — dangling pointer

```c
free(p);
printf("%d\n", *p);           // UB — memory may be reused
```

Fix: `free(p); p = NULL;` — at least you'll get a clean crash on NULL dereference.

### 3. Double free

```c
free(p);
free(p);                       // UB — corrupts allocator metadata
```

Fix: `free(p); p = NULL;` — `free(NULL)` is a no-op.

### 4. Buffer overflow — write past allocation

```c
int *p = malloc(10 * sizeof *p);
p[10] = 42;                    // UB — one past the end
```

Detection: ASan, Valgrind. Prevention: always track size alongside pointer.

## Idioms summary

| Pattern | When to use |
|---------|------------|
| `sizeof *ptr` in malloc | always — type-safe, adapts to type changes |
| `calloc` over `malloc` + `memset` | arrays — zero-init + overflow-safe |
| `tmp = realloc(); if (tmp) p = tmp;` | any resize — never lose original pointer |
| `free(p); p = NULL;` | always after free — prevents use-after-free and double-free |
| `Node **head` for linked list | any operation that may change the head |
| `**` for allocate-and-return | function allocates, caller receives via parameter |
| `void *` + function pointer | generic containers and callbacks |
| `restrict` on hot loop params | DSP, numerical code, performance-critical paths |
| flexible array member | variable-length data contiguous with header |
| init/destroy function pairs | any struct that owns heap memory |
