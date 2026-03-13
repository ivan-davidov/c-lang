# Structs, Unions, and Enums

## Structs

A struct groups related data into one block of memory:

```c
struct Point {
    int x;
    int y;
};
struct Point p = {10, 20};    // must say "struct Point" everywhere
```

### Struct tag vs typedef

```c
// tag only — must write "struct" everywhere
struct Point { int x; int y; };
struct Point p;

// typedef only — no tag, can't self-reference
typedef struct { int x; int y; } Point;
Point p;

// both — best of both worlds
typedef struct Point {
    int x;
    int y;
} Point;
// now both "struct Point" and "Point" work
```

### Why linked lists NEED a struct tag

```c
// BROKEN — typedef isn't complete yet when the struct body is parsed:
typedef struct {
    int data;
    Node *next;       // ERROR — "Node" doesn't exist yet
} Node;

// WORKS — struct tag exists as soon as the opening brace:
typedef struct Node {
    int data;
    struct Node *next; // OK — "struct Node" is known
} Node;
```

Same applies to any self-referencing structure: trees, graphs, doubly-linked lists:

```c
typedef struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;
```

### Mutual references — both need tags + forward declaration

```c
typedef struct A A;
typedef struct B B;

struct A {
    B *partner;    // OK — B is forward-declared
};
struct B {
    A *partner;    // OK — A is forward-declared
};
```

## Initialization

```c
// positional
struct Point p = {10, 20};

// designated (C99) — preferred, order doesn't matter, self-documenting
struct Point p = {.y = 20, .x = 10};

// zero-init everything
struct Point p = {0};

// compound literal (C99) — creates a temporary struct value
draw_point((struct Point){.x = 5, .y = 10});
```

## Accessing members

```c
struct Point p = {10, 20};
p.x                       // dot — direct access

struct Point *pp = &p;
pp->x                     // arrow — through pointer (shorthand for (*pp).x)
```

## Strings in structs — `char[]` vs `char *`

### Embedded array — `char name[32]`

```c
typedef struct {
    int id;
    char name[32];       // 32 bytes stored INSIDE the struct
} User;

User u = {1, "Alice"};   // "Alice\0" copied into the array
```

- struct is self-contained — one `malloc`, one `free`, one `memcpy` copies everything
- fixed max length — wastes space if most names are short, truncates if too long
- no dangling pointer risk — the string lives inside the struct
- **use for:** fixed-size fields, embedded systems, protocol packets, database records

### Pointer — `char *name`

```c
typedef struct {
    int id;
    char *name;          // 8 bytes (pointer) stored in struct, string lives elsewhere
} User;

User u = {1, strdup("Alice")};   // must allocate separately
// must free(u.name) before freeing u
```

- flexible length — no wasted space, no truncation
- must manage lifetime — who allocates? who frees? dangling pointer if source freed?
- copying the struct copies the pointer, not the string — two structs share the same string (aliasing bug)
- **use for:** dynamic data, variable-length strings, strings that outlive the struct

### The deep copy trap

```c
User a = {1, strdup("Alice")};
User b = a;                       // copies the pointer, NOT the string
free(a.name);                     // b.name is now dangling — points to freed memory
```

With `char name[32]`, `b = a` copies the entire array. Safe.

## Memory layout and alignment

### Why alignment exists

CPUs read memory in aligned chunks. A 4-byte `int` at address 0x1001 (not divisible by 4) straddles two chunks — the CPU needs two reads and a shift to assemble the value. On x86 this is just slow. On ARM it's a hardware fault — your program crashes.

So the compiler inserts invisible **padding bytes** to ensure every member starts at an address divisible by its own size.

### Rule 1: each member aligns to its own size

Not to the largest member. Each type has its own alignment requirement:

| Type | Size | Must start at address divisible by |
|------|------|------------------------------------|
| `char` | 1 | 1 (any address — no constraint) |
| `short` | 2 | 2 |
| `int` / `float` | 4 | 4 |
| `double` / pointer | 8 | 8 |

Think of padding as belonging to the **next** member, not the previous one. A `char` alone needs nothing after it — padding only appears because the next member needs a correctly aligned start address:

```c
struct { char a; char b; };           // 0 padding — b aligns to 1, any address works
struct { char a; int b; };            // 3 bytes before b — b needs 4-aligned
struct { char a; char b; int c; };    // 2 bytes before c — offset 2, c needs offset 4
```

Padding is inserted **before** a member to push it to its required alignment:

```c
struct Example {
    char a;       // offset 0   — size 1, aligns to 1 ✓
                  // 1 byte padding — because short needs offset divisible by 2
    short b;      // offset 2   — size 2, aligns to 2 ✓
    char c;       // offset 4   — size 1, aligns to 1 ✓
                  // 3 bytes padding — because int needs offset divisible by 4
    int d;        // offset 8   — size 4, aligns to 4 ✓
    char e;       // offset 12  — size 1, aligns to 1 ✓
                  // 3 bytes padding — because double needs offset divisible by 8? NO.
                  // actually 3 bytes padding to reach offset 16 for double
    double f;     // offset 16  — size 8, aligns to 8 ✓
};
```

Visualized byte-by-byte (64-bit system):

```
offset: 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
        [a][_][b  b][c][_ __ _][d  d  d  d][e][_  _  _  _  _  _  _][f  f  f  f  f  f  f  f]
         1  P  2     1  3 pad   4            1  7 padding            8

sizeof = 24
```

Notice: the padding before `d` is 3 bytes (to reach offset 8 which is divisible by 4). The padding before `f` is 3 bytes (to reach offset 16 which is divisible by 8). Each gap is determined by **what comes next**, not by the largest member.

### Rule 2: struct total size must be a multiple of its largest member's alignment

This is **trailing padding** — it exists so that arrays of structs work:

```c
struct Foo {
    double d;    // 8 bytes          offset 0
    char a;      // 1 byte           offset 8
                 // 7 bytes trailing padding
};
// sizeof = 16, not 9
```

Why? Imagine an array:
```
struct Foo arr[3];

if sizeof were 9:
  arr[0].d at 0x1000  — aligned ✓
  arr[1].d at 0x1009  — NOT divisible by 8 — broken ✗

with sizeof = 16 (padded to multiple of 8):
  arr[0].d at 0x1000  — aligned ✓
  arr[1].d at 0x1010  — aligned ✓
  arr[2].d at 0x1020  — aligned ✓
```

Without trailing padding, the second struct starts at a wrong address and every member inside it is misaligned. Trailing padding ensures `sizeof(struct)` is a stride that preserves alignment for all members in every subsequent array element.

### Summary of the two rules

1. **Between members:** pad so the next member starts at an address divisible by **its own** size
2. **At the end:** pad so the total struct size is a multiple of the **largest member's** alignment

The compiler never reorders members — padding is the only tool it has.

### Minimizing padding — order members largest to smallest

```c
// wasteful: 24 bytes
struct Padded {
    char a;       // 1 + 7 padding (double needs 8-aligned)
    double b;     // 8
    char c;       // 1 + 7 trailing padding (struct aligns to 8)
};

// tight: 16 bytes — same data, half the waste
struct Tight {
    double b;     // 8
    char a;       // 1
    char c;       // 1 + 6 trailing padding
};
```

Same members, different order, 8 bytes saved. On embedded with limited RAM this matters.

### Checking layout

```c
#include <stddef.h>
printf("size: %zu\n", sizeof(struct Example));
printf("offset of d: %zu\n", offsetof(struct Example, d));  // exact byte offset
```

Verify your understanding by printing offsets — if they don't match your mental model, you miscounted padding.

### Packed structs — disable padding (embedded/protocols)

```c
struct __attribute__((packed)) Packet {
    uint8_t type;
    uint32_t length;
    uint16_t checksum;
};
// sizeof = 7 (no padding) — matches wire format exactly
// but unaligned access may be slow or crash on ARM
```

Use only for serialization/hardware registers where layout must match an external format.

## Unions

All members share the same memory — only one is valid at a time:

```c
union Value {
    int i;
    float f;
    char *s;
};
// sizeof = max(sizeof members) = 8 (pointer)
// writing to .i then reading .f is UB (type punning — except char*)
```

### Tagged union — the C pattern for variant types

```c
typedef enum { VAL_INT, VAL_FLOAT, VAL_STR } ValType;

typedef struct {
    ValType type;         // tag — tells you which union member is valid
    union {
        int i;
        float f;
        char *s;
    };                    // anonymous union (C11) — access as val.i, not val.u.i
} Value;

Value v = {.type = VAL_INT, .i = 42};

switch (v.type) {
    case VAL_INT:   printf("%d\n", v.i); break;
    case VAL_FLOAT: printf("%f\n", v.f); break;
    case VAL_STR:   printf("%s\n", v.s); break;
}
```

This is everywhere: JSON parsers, AST nodes, event systems, protocol messages, embedded command buffers.

### Structs with matching first members in a union

C guarantees that if multiple structs in a union share the same initial members, you can read those common members through any of them:

```c
typedef struct { int type; int x; int y; } MoveEvent;
typedef struct { int type; int key; }      KeyEvent;

typedef union {
    int type;            // common first member — always readable
    MoveEvent move;
    KeyEvent key;
} Event;

Event e;
// can always read e.type to determine which struct to use
// this is guaranteed by the C standard (C11 §6.5.2.3)
```

This is how many C event systems work (SDL, X11, Win32 messages). The `type` field is at offset 0 in every variant, so you inspect it first to know which member to access.

## Enums

Named integer constants:

```c
typedef enum { RED, GREEN, BLUE } Color;    // RED=0, GREEN=1, BLUE=2
typedef enum { ERR_NONE = 0, ERR_IO = -1, ERR_MEM = -2 } Error;  // explicit values
```

- enums are just `int` — no type safety, you can assign any integer
- use with `switch` and omit `default` to get `-Wswitch` warnings for missing cases
- convention: `ALL_CAPS` or `PREFIX_NAME` (e.g. `COLOR_RED`)

### Enum for array sizing

```c
typedef enum {
    SENSOR_TEMP,
    SENSOR_HUMIDITY,
    SENSOR_PRESSURE,
    SENSOR_COUNT          // always last — equals the number of sensors
} SensorType;

float readings[SENSOR_COUNT];   // array sized by enum — grows automatically
```

## Idioms

### Opaque struct (encapsulation in C)

**What:** Forward-declare a struct in the header, define it only in the `.c` file.
**Why:** Users can't access or depend on internal fields — you can change the layout without breaking their code.
**When:** Library APIs, any module boundary where you want to hide implementation.
**Where:** SQLite (`sqlite3 *`), OpenSSL (`SSL_CTX *`), FreeRTOS (`TaskHandle_t`).

```c
// config.h (public)
typedef struct Config Config;          // user sees only the pointer type
Config *config_load(const char *path);
int config_get_port(const Config *c);
void config_free(Config *c);

// config.c (private)
struct Config {                        // layout hidden from users
    char *data;
    size_t len;
    int port;
};
```

### Struct of function pointers (vtable / interface)

**What:** A struct where every member is a function pointer — C's version of an interface or virtual method table.
**Why:** Swap implementations at runtime without changing calling code. One API, multiple backends.
**When:** Driver abstraction, plugin systems, testing (swap real driver for a mock).
**Where:** Linux kernel (`struct file_operations`), embedded HALs, any driver layer.

```c
typedef struct {
    int (*open)(const char *path);
    int (*read)(void *buf, size_t len);
    void (*close)(void);
} Driver;

// UART implementation
Driver uart_driver = {.open = uart_open, .read = uart_read, .close = uart_close};

// SPI implementation — same interface, different backend
Driver spi_driver = {.open = spi_open, .read = spi_read, .close = spi_close};

// calling code doesn't care which one:
void process(Driver *d) {
    d->open("/dev/sensor");
    d->read(buf, 64);
    d->close();
}
```

### Tagged union (algebraic data type / variant)

**What:** An `enum` tag + `union` — C's manual version of sum types / discriminated unions.
**Why:** Represent a value that can be one of several types, safely dispatched at runtime.
**When:** Any data that has variants — messages, events, AST nodes, config values, JSON.
**Where:** SDL events, X11 events, protocol parsers, interpreters, serialization.

```c
typedef enum { VAL_INT, VAL_FLOAT, VAL_STR } ValType;

typedef struct {
    ValType type;
    union {
        int i;
        float f;
        char *s;
    };
} Value;

// always check the tag before reading:
switch (v.type) {
    case VAL_INT:   printf("%d\n", v.i); break;
    case VAL_FLOAT: printf("%f\n", v.f); break;
    case VAL_STR:   printf("%s\n", v.s); break;
}
```

Unlike Rust/Haskell, the compiler won't stop you from reading `v.f` when `v.type == VAL_INT`. That's on you.

### Flexible array member

**What:** An unsized array as the last struct member — allocate header + data in one `malloc`.
**Why:** One allocation, one free, data is contiguous with the header (cache-friendly).
**When:** Variable-length records — network packets, file format records, message buffers.
**Where:** Linux kernel (`struct sk_buff`), protocol implementations, file systems.

```c
typedef struct {
    size_t len;
    char data[];              // must be last, no size specified
} Buffer;

Buffer *b = malloc(sizeof(Buffer) + 100);   // header + 100 bytes of data
b->len = 100;
memcpy(b->data, source, 100);
// one free() cleans up everything
free(b);
```

Cannot be stack-allocated or used as a value type — must be heap-allocated through a pointer.

### Designated initializers for config

**What:** Name the fields you're setting, let the rest default to zero.
**Why:** Self-documenting, order-independent, new fields added later auto-default to zero.
**When:** Initialization of structs with many fields where most have sensible defaults.
**Where:** Everywhere — library option structs, hardware configuration, test setup.

```c
Config cfg = {
    .port = 8080,
    .max_connections = 100,
    .verbose = true,
    // .timeout, .log_path, etc. — all zero/NULL by default
};
```

### Compound literal

**What:** Creates an unnamed temporary struct value inline — `(Type){initializer}`.
**Why:** Pass struct values to functions without declaring a variable first.
**When:** One-off values, default arguments, initializing pointers to stack-allocated structs.
**Where:** Function calls, assignments, anywhere a struct value is needed.

```c
void draw(Point p);
draw((Point){.x = 5, .y = 10});     // temporary, no variable needed
draw((Point){0});                     // zero-initialized

Point *p = &(Point){.x = 1, .y = 2}; // pointer to compound literal (lifetime = enclosing scope)
```

### Init/destroy pair (resource management)

**What:** Constructor/destructor functions that allocate and free a struct and its resources.
**Why:** C has no RAII or destructors — manual cleanup is the only option. Pairing them makes ownership explicit.
**When:** Any struct that owns heap memory, file handles, or hardware resources.
**Where:** Every C library — `fopen`/`fclose`, `pthread_create`/`pthread_join`, `malloc`/`free`.

```c
typedef struct {
    int fd;
    char *buf;
    size_t buf_size;
} Stream;

Stream *stream_open(const char *path) {
    Stream *s = malloc(sizeof *s);
    if (!s) return NULL;
    s->fd = open(path, O_RDONLY);
    if (s->fd < 0) { free(s); return NULL; }
    s->buf_size = 4096;
    s->buf = malloc(s->buf_size);
    if (!s->buf) { close(s->fd); free(s); return NULL; }
    return s;
}

void stream_close(Stream *s) {
    if (!s) return;
    free(s->buf);
    close(s->fd);
    free(s);
}
```

### Container struct (grouping related state)

**What:** Bundle related variables into a struct instead of passing 5 separate arguments.
**Why:** Cleaner function signatures, easier to extend, single pointer pass instead of copying N values.
**When:** When 3+ related values keep traveling together through functions.
**Where:** Embedded context structs, game state, parser state, connection handles.

```c
// instead of:
void render(int x, int y, int w, int h, float zoom, bool visible);

// bundle it:
typedef struct {
    int x, y, w, h;
    float zoom;
    bool visible;
} Viewport;

void render(const Viewport *vp);
```

### Bit-field struct (hardware registers)

**What:** Struct members with explicit bit widths.
**Why:** Map directly to hardware register layouts or protocol bit fields.
**When:** Register definitions, compact flags, protocol headers.
**Where:** Embedded firmware, network protocol parsing.

```c
typedef struct {
    uint8_t enabled : 1;      // 1 bit
    uint8_t mode    : 3;      // 3 bits — values 0..7
    uint8_t channel : 4;      // 4 bits — values 0..15
} ControlReg;                  // total: 1 byte, 8 bits, no waste

ControlReg reg = {.enabled = 1, .mode = 3, .channel = 5};
```

Caveat: bit ordering (MSB/LSB first) is implementation-defined — not portable across compilers/architectures. For portable code, use explicit bit masks instead.
