# Generics in C

C has no templates, no generics keyword, no parametric polymorphism. But the need is real — you don't want to write `stack_push_int`, `stack_push_float`, `stack_push_string` separately. Here are the approaches, from most common to least.

## Approach 1: `void *` (type erasure)

Cast everything to `void *`, store it, cast it back. The container doesn't know what it holds.

```c
// generic stack — stores any pointer type
typedef struct {
    void **data;
    int top;
    int capacity;
} Stack;

Stack *stack_create(int capacity) {
    Stack *s = malloc(sizeof *s);
    s->data = malloc(capacity * sizeof(void *));
    s->top = 0;
    s->capacity = capacity;
    return s;
}

void stack_push(Stack *s, void *item) {
    s->data[s->top++] = item;
}

void *stack_pop(Stack *s) {
    return s->data[--s->top];
}

// usage — caller casts:
int *x = malloc(sizeof *x);
*x = 42;
stack_push(s, x);

int *y = (int *)stack_pop(s);
printf("%d\n", *y);        // 42
free(y);
```

### `qsort` — the standard library example

```c
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
```

It sorts ANY array. It doesn't know the element type — it just knows the element size and calls your comparator.

```c
int compare_ints(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

int arr[] = {5, 2, 8, 1, 9};
qsort(arr, 5, sizeof(int), compare_ints);
// arr is now {1, 2, 5, 8, 9}
```

How `qsort` works internally: it swaps memory blobs of `size` bytes using `memcpy`, and calls `compar` to decide ordering. It never dereferences the data — it's just moving bytes around.

### `void *` for callbacks (the context pattern)

```c
// timer doesn't know what type the callback needs
typedef void (*Callback)(void *ctx);

void timer_set(int ms, Callback cb, void *ctx);

// caller defines their own context
typedef struct { int pin; int count; } BlinkCtx;

void blink(void *ctx) {
    BlinkCtx *b = (BlinkCtx *)ctx;
    toggle(b->pin);
    b->count++;
}

BlinkCtx my_blink = {.pin = 5, .count = 0};
timer_set(500, blink, &my_blink);
```

**Pros:** Works everywhere, no macros, runtime flexible.
**Cons:**
- **No type safety** — push an `int *`, pop and cast to `float *` → silent corruption
- **Requires heap allocation** for value types (can't push a bare `int`, must malloc it)
- **Extra indirection** — storing pointers to data, not data itself

### Storing values directly with `memcpy`

You can avoid the heap-allocation-per-element problem by copying values into the container:

```c
typedef struct {
    char *data;          // raw byte buffer
    size_t elem_size;    // size of one element
    size_t count;
    size_t capacity;
} Vector;

Vector *vec_create(size_t elem_size, size_t initial_cap) {
    Vector *v = malloc(sizeof *v);
    v->elem_size = elem_size;
    v->data = malloc(elem_size * initial_cap);
    v->count = 0;
    v->capacity = initial_cap;
    return v;
}

void vec_push(Vector *v, const void *elem) {
    if (v->count >= v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, v->elem_size * v->capacity);
    }
    memcpy(v->data + v->count * v->elem_size, elem, v->elem_size);
    v->count++;
}

void *vec_get(Vector *v, size_t index) {
    return v->data + index * v->elem_size;
}

// usage:
Vector *v = vec_create(sizeof(int), 16);
int x = 42;
vec_push(v, &x);                          // copies the int in
int *y = (int *)vec_get(v, 0);            // get pointer to stored copy
printf("%d\n", *y);                       // 42
```

This is how most serious C containers work — store values contiguously, use `elem_size` for pointer arithmetic, caller casts on retrieval.

## Approach 2: Macros (compile-time code generation)

Generate type-specific code with the preprocessor. The macro expands to real, typed code — no `void *`, no casts.

### Type-parameterized macro

```c
#define DEFINE_STACK(T, Name)                                \
    typedef struct {                                         \
        T *data;                                             \
        int top, capacity;                                   \
    } Name;                                                  \
                                                             \
    static inline Name Name##_create(int cap) {              \
        Name s = { .data = malloc(cap * sizeof(T)),          \
                   .top = 0, .capacity = cap };              \
        return s;                                            \
    }                                                        \
    static inline void Name##_push(Name *s, T val) {         \
        s->data[s->top++] = val;                             \
    }                                                        \
    static inline T Name##_pop(Name *s) {                    \
        return s->data[--s->top];                            \
    }                                                        \
    static inline bool Name##_empty(const Name *s) {         \
        return s->top == 0;                                  \
    }

// instantiate for specific types:
DEFINE_STACK(int, IntStack)
DEFINE_STACK(double, DblStack)
DEFINE_STACK(Point, PointStack)

// usage — fully type-safe:
IntStack s = IntStack_create(100);
IntStack_push(&s, 42);
int x = IntStack_pop(&s);             // no cast needed, type is int
```

**Pros:** Full type safety, no overhead, no heap allocation per element, inline-able.
**Cons:**
- **Macro nightmare** — debugging is pain (error messages point to expanded code)
- **Code bloat** — every instantiation duplicates the entire implementation
- **Multi-line macros** are fragile and hard to read

### X-macros (table-driven code generation)

Generate repetitive code from a list:

```c
// define the list once:
#define SENSOR_LIST \
    X(TEMP,     "temperature",  read_temp)    \
    X(HUMIDITY,  "humidity",     read_humid)   \
    X(PRESSURE,  "pressure",    read_press)

// generate an enum:
typedef enum {
    #define X(id, name, fn) SENSOR_##id,
    SENSOR_LIST
    #undef X
    SENSOR_COUNT     // sentinel = number of sensors
} SensorId;

// generate a name table:
static const char *sensor_names[] = {
    #define X(id, name, fn) name,
    SENSOR_LIST
    #undef X
};

// generate a function pointer table:
typedef float (*ReadFn)(void);
static ReadFn sensor_readers[] = {
    #define X(id, name, fn) fn,
    SENSOR_LIST
    #undef X
};

// now adding a sensor = one line in SENSOR_LIST
// the enum, name table, and reader table all stay in sync automatically
```

**Where:** Linux kernel (system call tables), protocol parsers (opcode dispatch), embedded (register definitions, error code tables).

## Approach 3: `_Generic` (C11)

Compile-time type dispatch — like a type switch that resolves at compile time.

```c
#define abs_val(x) _Generic((x), \
    int:     abs,                 \
    long:    labs,                \
    float:   fabsf,              \
    double:  fabs                \
)(x)

int a = abs_val(-5);        // calls abs(-5)
double b = abs_val(-3.14);  // calls fabs(-3.14)
```

```c
#define print_val(x) _Generic((x),       \
    int:       printf("%d\n", (x)),       \
    double:    printf("%f\n", (x)),       \
    char *:    printf("%s\n", (x)),       \
    default:   printf("unknown type\n")   \
)

print_val(42);              // prints "42"
print_val(3.14);            // prints "3.140000"
print_val("hello");         // prints "hello"
```

**Pros:** Type-safe, no runtime overhead, standard C11.
**Cons:** Can only dispatch on type, not arity. You must list every type explicitly. Combining with macro-generated functions is verbose. Rarely used in practice.

## Approach 4: `container_of` (Linux kernel pattern)

Instead of storing `void *`, embed a generic "link" struct inside your data. Then recover the outer struct from the link using pointer arithmetic.

```c
#include <stddef.h>

// the magic macro — given a pointer to a member, get a pointer to the containing struct
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// generic linked list node — contains only prev/next, no data
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

// embed ListNode in your actual data structs:
typedef struct {
    int x, y;
    ListNode node;          // intrusive link — embedded, not pointed to
} Point;

typedef struct {
    char name[64];
    ListNode node;          // same ListNode works for any struct
} User;

// list operations work on ListNode — type-agnostic
void list_insert(ListNode *head, ListNode *new_node) {
    new_node->next = head->next;
    new_node->prev = head;
    head->next->prev = new_node;
    head->next = new_node;
}

// recovering the outer struct:
void print_points(ListNode *head) {
    for (ListNode *n = head->next; n != head; n = n->next) {
        Point *p = container_of(n, Point, node);
        printf("(%d, %d)\n", p->x, p->y);
    }
}
```

**How `container_of` works:** If you know the member's offset within the struct, subtract that offset from the member's address to get the struct's address. `offsetof(Point, node)` gives the byte offset of `node` within `Point`.

**Pros:** No `void *` casts, no extra allocation, data and links are one allocation, type-safe (the cast is localized to `container_of`), cache-friendly.
**Cons:** Invasive — your data type must include the list node. Slightly mind-bending until you internalize it.
**Where:** Linux kernel uses this for EVERYTHING — `list_head`, `rb_node`, `hlist_node`. It's the foundation of the kernel's data structures.

## When to use which

| Approach | Type safety | Overhead | Complexity | Best for |
|----------|------------|----------|------------|----------|
| `void *` + cast | none | pointer indirection | low | quick utilities, callbacks |
| `void *` + `memcpy` | none | memcpy cost | medium | value containers (vectors) |
| Macro generation | full | zero | high (macros) | performance-critical types |
| `_Generic` | full | zero | medium | type-dispatch wrappers |
| `container_of` | full | zero | high (mental) | intrusive containers, kernel |

For your projects: start with `void *` for the ring buffer (it's the simplest). If you later need type-specific containers, try the macro approach. Know `container_of` exists for when you read Linux kernel code.
