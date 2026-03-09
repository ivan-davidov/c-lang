# Program Organization

## Compilation units

- each `.c` file is compiled independently into a `.o` (object file)
- linker combines `.o` files into final binary
- a `.c` file + everything it `#include`s = one **translation unit**

## Headers (`.h`)

What goes in a header:
- function declarations (prototypes)
- type definitions (`struct`, `enum`, `typedef`)
- macros (`#define`)
- `extern` variable declarations
- `static inline` function definitions

What does NOT go in a header:
- function definitions (causes duplicate symbol errors when included by multiple `.c` files)
- variable definitions (same problem)

## Include guards

Every header needs one — prevents double inclusion:

```c
// config.h
#ifndef CONFIG_H
#define CONFIG_H

// declarations here

#endif
```

Or the non-standard but universally supported shortcut:
```c
#pragma once
```

## Linkage and storage

| Keyword | On a function | On a variable (file scope) | On a variable (block scope) |
|---------|--------------|---------------------------|----------------------------|
| (none) | visible everywhere (external linkage) | visible everywhere (external linkage) | local, created/destroyed per call |
| `static` | visible only in this file | visible only in this file | local but persists across calls |
| `extern` | declaration only (defined elsewhere) | declaration only (defined elsewhere) | — |

### `static` — two different meanings depending on where

```c
// file scope: internal linkage — "private to this file"
static int counter = 0;
static void helper(void) { /* ... */ }

// block scope: persistent storage — "remember value between calls"
void tick(void) {
    static int count = 0;   // initialized once, survives across calls
    count++;
    printf("%d\n", count);  // 1, 2, 3, ... on successive calls
}
```

### `extern` — "this exists, but it's defined in another file"

```c
// globals.h
extern int verbose;        // declaration — no memory allocated

// globals.c
int verbose = 0;           // definition — memory allocated here, once

// main.c
#include "globals.h"
if (verbose) printf("...");
```

## The standard multi-file pattern

```
project/
├── main.c         // entry point, includes module headers
├── parser.h       // declarations: struct types, function prototypes
├── parser.c       // definitions: #include "parser.h", implements functions
├── util.h
└── util.c
```

```c
// parser.h
#ifndef PARSER_H
#define PARSER_H

typedef struct {
    char *fields[32];
    int count;
} Row;

int parse_line(const char *line, Row *out);

#endif

// parser.c
#include "parser.h"
#include <string.h>

static char *skip_spaces(char *s) { /* ... */ }  // private to this file

int parse_line(const char *line, Row *out) {
    // implementation
}
```

## Idioms

- **one header per `.c` file** — `foo.c` has `foo.h`, keeps things findable
- **`.c` includes its own header first** — catches missing includes in the header itself
- **`static` everything by default** — only expose in header what other files need
- **no globals if possible** — pass state through function parameters or structs. When unavoidable, `extern` in header, definition in one `.c`
- **forward declarations** for mutual dependencies:
  ```c
  typedef struct B B;      // forward declare B
  typedef struct A {
      B *b;
  } A;
  typedef struct B {
      A *a;
  } B;
  ```
