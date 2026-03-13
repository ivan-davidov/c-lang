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

The compiler inserts **padding bytes** to align members to their natural boundaries:

```c
struct Bad {
    char a;      // 1 byte
                 // 3 bytes padding ← compiler inserts this
    int b;       // 4 bytes
};
// sizeof(struct Bad) = 8, not 5

struct Good {
    int b;       // 4 bytes
    char a;      // 1 byte
                 // 3 bytes trailing padding (struct size must be multiple of largest alignment)
};
// sizeof(struct Good) = 8 — same here, but different internal layout
```

### Why alignment exists

CPUs read memory in chunks (4 or 8 bytes). An `int` at address 0x1001 (not divisible by 4) requires two reads and a shift — slow or even illegal on some architectures (ARM will fault).

### Rules

- each member aligns to its own size: `char` = 1, `short` = 2, `int` = 4, `double`/pointer = 8
- struct total size is padded to a multiple of its largest member's alignment
- members are laid out in declaration order — compiler never reorders

### Minimizing padding — order members largest to smallest

```c
// wasteful: 24 bytes (on 64-bit)
struct Padded {
    char a;       // 1 + 7 padding
    double b;     // 8
    char c;       // 1 + 7 padding
};

// tight: 16 bytes
struct Packed {
    double b;     // 8
    char a;       // 1
    char c;       // 1 + 6 padding
};
```

### Checking layout

```c
#include <stddef.h>
printf("size: %zu\n", sizeof(struct Foo));
printf("offset of b: %zu\n", offsetof(struct Foo, b));  // exact byte offset of member
```

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

- **opaque struct** — hide implementation:
  ```c
  // header
  typedef struct Config Config;
  Config *config_load(const char *path);
  void config_free(Config *c);

  // source
  struct Config { char *data; size_t len; };
  ```

- **struct of function pointers** — C "interface" / vtable:
  ```c
  typedef struct {
      int (*open)(const char *path);
      int (*read)(void *buf, size_t len);
      void (*close)(void);
  } Driver;

  Driver uart_driver = {.open = uart_open, .read = uart_read, .close = uart_close};
  uart_driver.open("/dev/ttyS0");
  ```

- **flexible array member** (C99) — variable-length data at end of struct:
  ```c
  typedef struct {
      size_t len;
      char data[];          // must be last member, no size
  } Buffer;

  Buffer *b = malloc(sizeof(Buffer) + 100);
  b->len = 100;
  // b->data[0..99] is usable
  ```
  one allocation for header + data. Common in network packets, file formats, kernel structures.

- **designated initializers for config**:
  ```c
  Config cfg = {
      .port = 8080,
      .max_connections = 100,
      .verbose = true,
      // unspecified members are zero-initialized
  };
  ```

- **compound literal as default**:
  ```c
  void draw(Point p);
  draw((Point){0});           // zero-initialized temporary
  draw((Point){.x = 5});     // y defaults to 0
  ```
