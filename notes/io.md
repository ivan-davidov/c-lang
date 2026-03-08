# I/O — printf, scanf, and friends

## Output functions

| Function | What it does | Use when |
|----------|-------------|----------|
| `printf(fmt, ...)` | formatted output to stdout | most of the time |
| `putchar(c)` | write one char to stdout | single character output, tight loops |
| `puts(s)` | write string + newline to stdout | quick string output, no formatting needed |
| `fprintf(fp, fmt, ...)` | formatted output to any stream | writing to files, stderr |
| `snprintf(buf, n, fmt, ...)` | formatted output to buffer with size limit | building strings safely |
| `sprintf(buf, fmt, ...)` | formatted output to buffer, NO size limit | **avoid — buffer overflow risk** |

## Input functions

| Function | What it does | Use when |
|----------|-------------|----------|
| `getchar()` | read one char from stdin (returns `int`) | char-by-char processing |
| `fgets(buf, n, fp)` | read line into buffer with size limit | **always use this for line input** |
| `scanf(fmt, ...)` | formatted input from stdin | simple numeric input, prototyping |
| `fscanf(fp, fmt, ...)` | formatted input from any stream | parsing structured file data |
| `gets(buf)` | read line, NO size limit | **never — removed in C11, buffer overflow guaranteed** |

## printf format specifiers

### The basics

| Specifier | Type | Example |
|-----------|------|---------|
| `%d` | `int` | `printf("%d", 42)` → `42` |
| `%u` | `unsigned int` | `printf("%u", 42u)` → `42` |
| `%ld` | `long` | `printf("%ld", 100000L)` |
| `%lld` | `long long` | `printf("%lld", 1LL << 40)` |
| `%f` | `double` (and `float` — promoted) | `printf("%f", 3.14)` → `3.140000` |
| `%e` | `double` in scientific notation | `printf("%e", 3.14)` → `3.140000e+00` |
| `%g` | `double`, shorter of `%f` or `%e` | `printf("%g", 3.14)` → `3.14` |
| `%c` | `char` (passed as `int`) | `printf("%c", 'A')` → `A` |
| `%s` | `char *` (null-terminated) | `printf("%s", "hello")` |
| `%p` | `void *` (pointer address) | `printf("%p", (void *)ptr)` |
| `%x` | `unsigned int` as hex | `printf("%x", 255)` → `ff` |
| `%X` | same, uppercase | `printf("%X", 255)` → `FF` |
| `%o` | `unsigned int` as octal | `printf("%o", 8)` → `10` |
| `%%` | literal `%` | `printf("100%%")` → `100%` |

### Size modifiers (Ch 7)

| Modifier | Meaning | Example |
|----------|---------|---------|
| `%hhd` | `char` (as number) | `printf("%hhd", (char)42)` |
| `%hd` | `short` | |
| `%ld` | `long` | |
| `%lld` | `long long` | |
| `%zu` | `size_t` | `printf("%zu", sizeof(int))` — **the correct one** |
| `%td` | `ptrdiff_t` | pointer subtraction result |
| `%lf` | `double` in scanf (not needed in printf) | `scanf("%lf", &d)` |

### Fixed-width types (`<inttypes.h>`)

```c
#include <inttypes.h>
uint32_t val = 0xDEADBEEF;
printf("%" PRIu32 "\n", val);    // decimal
printf("%" PRIx32 "\n", val);    // hex
printf("%" PRId64 "\n", big);    // int64_t
```
Ugly but portable — `PRIu32` expands to the right modifier for the platform.

### Width and precision

```c
printf("%10d", 42);       //  "        42"  right-aligned, width 10
printf("%-10d", 42);      //  "42        "  left-aligned
printf("%05d", 42);       //  "00042"       zero-padded
printf("%.2f", 3.14159);  //  "3.14"        2 decimal places
printf("%.5s", "hello world"); // "hello"   truncate string to 5 chars
printf("%*d", width, val);//  width from variable — useful for tables
```

## scanf — quirks that matter

- **always pass pointers**: `scanf("%d", &x)` — forgetting `&` is UB, no warning
- **returns count of items matched** — check it:
  ```c
  if (scanf("%d", &x) != 1) {
      fprintf(stderr, "invalid input\n");
  }
  ```
- **`%d` vs `%i`**: `%d` reads decimal only, `%i` accepts `0x` (hex) and `0` (octal) prefixes — use `%d`
- **whitespace**: `scanf("%d %d", &a, &b)` — space in format matches any amount of whitespace including newlines
- **`%c` does NOT skip whitespace** — reads the next char including `\n` left by previous scanf:
  ```c
  scanf("%d", &n);    // user types "42\n" — \n stays in buffer
  scanf("%c", &c);    // reads the leftover \n, not the next real char
  scanf(" %c", &c);   // space before %c skips whitespace — the fix
  ```
- **`%s` stops at whitespace** and has no length limit — buffer overflow:
  ```c
  char buf[10];
  scanf("%s", buf);     // dangerous — input "hello world" overflows if > 9 chars
  scanf("%9s", buf);    // safer — limits to 9 chars + null terminator
  ```
- **newline left in buffer** is the #1 scanf headache — mixing scanf with fgets breaks because scanf leaves `\n` behind

## getchar / putchar idioms

- `getchar()` returns `int`, not `char` — must use `int` to detect `EOF`:
  ```c
  int c;  // NOT char c — EOF doesn't fit in char
  while ((c = getchar()) != EOF) {
      putchar(c);
  }
  ```
- copy stdin to stdout (simplest filter):
  ```c
  int c;
  while ((c = getchar()) != EOF)
      putchar(c);
  ```
- skip whitespace:
  ```c
  while ((c = getchar()) != EOF && isspace(c))
      ;  // empty body — intentional
  ```
- count lines:
  ```c
  int lines = 0;
  while ((c = getchar()) != EOF)
      if (c == '\n') lines++;
  ```

## Safe alternatives — use these in real code

| Instead of | Use | Why |
|-----------|-----|-----|
| `gets()` | `fgets(buf, sizeof buf, stdin)` | `gets` has no length limit — removed from C11 |
| `sprintf()` | `snprintf(buf, sizeof buf, fmt, ...)` | prevents buffer overflow |
| `scanf("%s")` | `fgets()` + `sscanf()` or manual parsing | control over buffer size |
| `scanf()` in general | `fgets()` + `strtol()` / `strtod()` | better error handling, no leftover newline |

### The `fgets` + parse pattern — the right way to read input

```c
char line[256];
if (fgets(line, sizeof line, stdin) != NULL) {
    int val;
    if (sscanf(line, "%d", &val) == 1) {
        // got a valid integer
    }
}
```
Why: `fgets` reads the whole line including `\n`, no buffer issues. `sscanf` parses from the string. Clean separation of reading and parsing.

### `strtol` for robust number parsing

```c
char *end;
long val = strtol(line, &end, 10);
if (end == line) {
    // no digits found
} else if (*end != '\0' && *end != '\n') {
    // trailing garbage after number
}
```
Better than `sscanf("%d")` — gives you exact error position and handles overflow via `errno`.

## stderr — errors go here, not stdout

```c
fprintf(stderr, "error: could not open %s\n", filename);
```
Keeps error messages separate from program output — matters when piping: `./prog < input > output` still shows errors on terminal.
