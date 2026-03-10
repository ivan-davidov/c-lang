# Strings

## What a string is in C

There is no string type. A string is a `char` array terminated by `'\0'` (null byte, value 0):

```c
char s[] = "hello";
// in memory: ['h']['e']['l']['l']['o']['\0']  — 6 bytes, not 5
```

Everything about C strings follows from this: length is discovered by scanning for `'\0'`, there's no bounds checking, and forgetting the terminator is a bug that corrupts memory.

## String literals

```c
"hello"              // stored in read-only memory, type: char[6] (including '\0')
```

- string literals may share storage — `"hello"` in two places might be the same address
- modifying a string literal is UB:
  ```c
  char *p = "hello";
  p[0] = 'H';         // UB — might segfault, might silently corrupt
  ```
- adjacent literals auto-concatenate:
  ```c
  "hello " "world"    // becomes "hello world" — useful for long strings
  ```

## `char[]` vs `char *`

This is the most important distinction:

```c
char arr[] = "hello";     // array on stack — COPY of the literal, 6 bytes, mutable
char *ptr = "hello";      // pointer to read-only literal — NOT a copy, just an address
```

| | `char arr[]` | `char *ptr` |
|---|---|---|
| Memory | stack (copy) | pointer to read-only section |
| Mutable? | yes — `arr[0] = 'H'` is fine | no — modifying is UB |
| `sizeof` | array size (6) | pointer size (8) |
| Reassignable? | no — `arr = other` is error | yes — `ptr = other` is fine |

**Rule:** use `char[]` when you need to modify the string. Use `const char *` when you just need to read it.

## `<string.h>` — the essential functions

### Length and comparison

| Function | What it does | Notes |
|----------|-------------|-------|
| `strlen(s)` | count chars until `'\0'` | O(n) every time — cache the result if looping |
| `strcmp(a, b)` | compare two strings | returns 0 if equal, <0 if a<b, >0 if a>b |
| `strncmp(a, b, n)` | compare first n chars | bounded — safer for partial matches |

```c
if (strcmp(cmd, "quit") == 0)    // string equality — NOT cmd == "quit"
```

`==` compares addresses, not contents. Always `strcmp`.

### Copy and concatenate

| Function | What it does | Danger |
|----------|-------------|--------|
| `strcpy(dst, src)` | copy src to dst | no bounds check — buffer overflow |
| `strncpy(dst, src, n)` | copy at most n chars | may NOT null-terminate if src >= n |
| `strcat(dst, src)` | append src to end of dst | no bounds check |
| `strncat(dst, src, n)` | append at most n chars | safer but awkward — n is max chars to append, not buffer size |

**`strncpy` does NOT do what you think:**
```c
char buf[10];
strncpy(buf, "hello world", 10);   // buf is NOT null-terminated — src was longer than 10
buf[9] = '\0';                      // must manually terminate
```
It also zero-fills the remainder if src is shorter — wasteful for large buffers.

**Prefer `snprintf` for safe string building:**
```c
char buf[64];
snprintf(buf, sizeof buf, "%s/%s", dir, file);   // always null-terminates, respects size
```

### Search

| Function | What it does |
|----------|-------------|
| `strchr(s, c)` | find first occurrence of char c — returns pointer or NULL |
| `strrchr(s, c)` | find last occurrence |
| `strstr(haystack, needle)` | find substring — returns pointer to start or NULL |
| `strtok(s, delim)` | split string by delimiters — destructive, uses static state |

**`strtok` is weird and dangerous:**
```c
char line[] = "one,two,three";
char *tok = strtok(line, ",");     // returns "one", modifies line (replaces , with \0)
while (tok) {
    printf("%s\n", tok);
    tok = strtok(NULL, ",");       // NULL means "continue from where you left off"
}
```
- destroys the original string (inserts `'\0'` at each delimiter)
- uses internal static state — not thread-safe, can't nest calls
- `strtok_r` is the reentrant (thread-safe) version (POSIX, not standard C)

### Conversion

| Function | What it does | Prefer over |
|----------|-------------|-------------|
| `strtol(s, &end, base)` | string to long, with error detection | `atoi` |
| `strtod(s, &end)` | string to double, with error detection | `atof` |
| `atoi(s)` | string to int — no error detection | avoid |
| `atof(s)` | string to double — no error detection | avoid |

```c
char *end;
long val = strtol("42abc", &end, 10);
// val = 42, *end = 'a' — tells you exactly where parsing stopped
```

## `<ctype.h>` — character classification

All take `int` (usually a `char` cast to `unsigned char`):

| Function | True for |
|----------|---------|
| `isalpha(c)` | a-z, A-Z |
| `isdigit(c)` | 0-9 |
| `isalnum(c)` | alpha or digit |
| `isspace(c)` | space, tab, newline, etc. |
| `isupper(c)` / `islower(c)` | uppercase / lowercase |
| `toupper(c)` / `tolower(c)` | convert case (returns int) |

**The `unsigned char` trap:**
```c
char c = -1;           // valid on platforms where char is signed
isalpha(c);            // UB — negative value passed to ctype function
isalpha((unsigned char)c);  // correct
```

## Memory layout of strings

```
char *argv[] = {"./prog", "hello", "world", NULL};

argv:   [ptr0] [ptr1] [ptr2] [NULL]
          |      |      |
          v      v      v
        "./prog\0"  "hello\0"  "world\0"
```

This is how `main(int argc, char **argv)` works — array of pointers to null-terminated strings, with a NULL sentinel at the end.

## Idioms

- **string equality:**
  ```c
  if (strcmp(a, b) == 0)        // never use ==
  ```

- **safe string building — always `snprintf`:**
  ```c
  snprintf(buf, sizeof buf, "error: %s at line %d", msg, line);
  ```

- **walk a string character by character:**
  ```c
  for (const char *p = s; *p; p++)    // *p is false when '\0' reached
      putchar(*p);
  ```

- **duplicate a string (POSIX `strdup`, not standard C):**
  ```c
  char *copy = strdup(s);    // malloc + strcpy — caller must free
  ```
  if you need portable:
  ```c
  char *copy = malloc(strlen(s) + 1);   // +1 for '\0'
  strcpy(copy, s);
  ```

- **prefix/suffix check:**
  ```c
  if (strncmp(s, "http://", 7) == 0)          // starts with
  if (strcmp(s + len - 4, ".txt") == 0)        // ends with (check len >= 4 first)
  ```

- **tokenize without destroying original:**
  ```c
  char *copy = strdup(line);    // work on a copy
  char *tok = strtok(copy, ",");
  // ...
  free(copy);
  ```

- **empty string check:**
  ```c
  if (s[0] == '\0')      // or: if (*s == '\0')  or: if (strlen(s) == 0)
  // first two are O(1), strlen is O(n) — prefer s[0] check
  ```
