# Error Handling in C

C has no exceptions, no `Result<T>` type, no `try`/`catch`. Error handling is manual and convention-based. Every C project picks a pattern and sticks to it.

## Pattern 1: Return codes (the dominant pattern)

Return an integer: 0 = success, nonzero = error. The actual result goes through an output parameter.

```c
int parse_config(const char *path, Config *out) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    if (!read_header(f, out)) {
        fclose(f);
        return -2;
    }

    fclose(f);
    return 0;
}

// caller:
Config cfg;
int err = parse_config("app.cfg", &cfg);
if (err) {
    fprintf(stderr, "config error: %d\n", err);
    exit(1);
}
```

### With named error codes

```c
typedef enum {
    ERR_OK          =  0,
    ERR_NULL_ARG    = -1,
    ERR_OPEN_FAIL   = -2,
    ERR_PARSE_FAIL  = -3,
    ERR_NO_MEMORY   = -4,
} ErrorCode;

const char *err_str(ErrorCode e) {
    switch (e) {
        case ERR_OK:         return "success";
        case ERR_NULL_ARG:   return "null argument";
        case ERR_OPEN_FAIL:  return "failed to open file";
        case ERR_PARSE_FAIL: return "parse error";
        case ERR_NO_MEMORY:  return "out of memory";
        default:             return "unknown error";
    }
}

// caller:
ErrorCode err = parse_config("app.cfg", &cfg);
if (err != ERR_OK) {
    fprintf(stderr, "error: %s\n", err_str(err));
}
```

**Where:** POSIX (most functions return 0/-1), SQLite (`SQLITE_OK`, `SQLITE_ERROR`, ...), ESP-IDF (`ESP_OK`, `ESP_FAIL`, `ESP_ERR_*`), FreeRTOS (`pdPASS`, `pdFAIL`).

### Return pointer: `NULL` = error

For functions that create/find something:

```c
Stack *stack_create(int capacity) {
    Stack *s = malloc(sizeof *s);
    if (!s) return NULL;              // allocation failed
    s->data = malloc(capacity * sizeof *s->data);
    if (!s->data) { free(s); return NULL; }
    s->top = 0;
    s->capacity = capacity;
    return s;
}

// caller:
Stack *s = stack_create(100);
if (!s) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}
```

**Where:** `malloc`, `fopen`, `strdup`, `getenv` — all return `NULL` on failure.

### Return bool: success/failure

When you don't need to distinguish error types:

```c
bool uart_send(const uint8_t *data, size_t len) {
    if (!data || len == 0) return false;
    // ... transmit ...
    return true;
}
```

## Pattern 2: `errno` (POSIX / standard library)

A global (thread-local) variable set by library functions when they fail. The function itself returns a sentinel value (-1, `NULL`, `EOF`), then you check `errno` for details.

```c
#include <errno.h>
#include <string.h>

FILE *f = fopen("/nonexistent", "r");
if (!f) {
    // errno is set by fopen
    printf("error %d: %s\n", errno, strerror(errno));
    // "error 2: No such file or directory"

    perror("fopen");
    // "fopen: No such file or directory" (prints to stderr)
}
```

### `errno` rules

- **always check the return value FIRST** — `errno` is only valid if the function indicated failure
- **set `errno = 0` before the call** if the function can legitimately return a value that looks like an error (e.g., `strtol` returns 0 both on success and failure)
- `errno` is **thread-local** in modern C (C11) — each thread has its own copy
- `errno` is an `int`, but its values are platform-specific (`ENOENT`, `ENOMEM`, `EACCES`, ...)
- **never use `errno` with non-POSIX functions** — only standard library and POSIX functions set it

```c
// strtol edge case — must set errno = 0 first
errno = 0;
char *end;
long val = strtol("abc", &end, 10);
if (errno != 0) {
    perror("strtol");          // range error (ERANGE)
} else if (end == str) {
    printf("no digits found\n");
} else {
    printf("value: %ld\n", val);
}
```

### Common `errno` values

| Name | Value | Meaning |
|------|-------|---------|
| `ENOENT` | 2 | No such file or directory |
| `EACCES` | 13 | Permission denied |
| `ENOMEM` | 12 | Out of memory |
| `EINVAL` | 22 | Invalid argument |
| `ERANGE` | 34 | Result out of range |
| `EAGAIN` | 11 | Try again (non-blocking I/O) |
| `EBADF` | 9 | Bad file descriptor |
| `EINTR` | 4 | Interrupted by signal |

## Pattern 3: Cleanup with `goto`

The one legitimate use of `goto` in C. When a function acquires multiple resources, early errors need to clean up what was already acquired — in reverse order.

### Without `goto` (pyramid of doom)

```c
int init_system(void) {
    int *buf = malloc(1024);
    if (!buf) return -1;

    FILE *log = fopen("log.txt", "w");
    if (!log) {
        free(buf);           // clean up buf
        return -2;
    }

    int fd = open("/dev/sensor", O_RDONLY);
    if (fd < 0) {
        fclose(log);         // clean up log
        free(buf);           // clean up buf
        return -3;
    }

    // success — use resources
    return 0;
}
```

Every error path repeats all previous cleanup. Adding a 4th resource means updating 3 error paths.

### With `goto` (linear cleanup)

```c
int init_system(void) {
    int ret = -1;

    int *buf = malloc(1024);
    if (!buf) goto err_buf;

    FILE *log = fopen("log.txt", "w");
    if (!log) goto err_log;

    int fd = open("/dev/sensor", O_RDONLY);
    if (fd < 0) goto err_fd;

    // success — use resources
    ret = 0;
    return ret;              // normal return, no cleanup needed

err_fd:
    fclose(log);
err_log:
    free(buf);
err_buf:
    return ret;
}
```

Each label falls through to the one below — cleanup happens in reverse acquisition order. Adding a resource = one `goto` label + one cleanup line.

**Where:** Linux kernel uses this everywhere. It's the standard pattern in kernel code for functions that acquire multiple resources. Search any kernel `.c` file for `goto err` or `goto out`.

## Pattern 4: `assert` (debug-only checks)

`assert(expr)` aborts the program with a message if `expr` is false. Disabled in release builds (`-DNDEBUG`).

```c
#include <assert.h>

void stack_push(Stack *s, int value) {
    assert(s != NULL);                    // programmer error — should never happen
    assert(!stack_full(s));               // precondition violation
    s->data[s->top++] = value;
}
```

### When to assert vs when to return an error

| Situation | Use |
|-----------|-----|
| Programmer bug (null pointer, invalid state) | `assert` — crash loudly |
| Runtime condition (file not found, network down) | Return error code — handle gracefully |
| Should-never-happen invariant | `assert` — if it fires, the code is wrong |
| User input validation | Return error — users are unpredictable |

```c
// assert: internal logic error — this means your code has a bug
assert(index < array_size);

// return error: external condition — not a bug, just bad luck
if (!fopen(path, "r")) return ERR_OPEN_FAIL;
```

In embedded without `stderr`, you can define your own assert that blinks an LED or triggers a breakpoint:

```c
#define ASSERT(cond) do { \
    if (!(cond)) { \
        __disable_irq(); \
        while (1) { toggle_led(LED_ERR); delay_ms(100); } \
    } \
} while(0)
```

## Pattern 5: `setjmp` / `longjmp` (non-local jump)

C's version of exceptions — jump back to a saved point, unwinding the call stack. Rarely used but worth knowing exists.

```c
#include <setjmp.h>

jmp_buf jump_target;

void dangerous_operation(void) {
    // ... deep in the call stack ...
    if (catastrophic_failure)
        longjmp(jump_target, 1);     // jump back to setjmp, return 1
}

int main(void) {
    if (setjmp(jump_target) != 0) {
        // we got here via longjmp — handle the error
        printf("recovered from catastrophic failure\n");
        return 1;
    }

    // normal execution
    dangerous_operation();           // might longjmp back
    return 0;
}
```

**Gotchas:**
- does NOT run destructors or cleanup — resources allocated between `setjmp` and `longjmp` are leaked
- local variables in the `setjmp` function may have indeterminate values after `longjmp` (unless `volatile`)
- hard to reason about, hard to maintain

**Where:** Lua uses `setjmp`/`longjmp` for its error handling. Some embedded systems use it for fatal error recovery. The PNG library (`libpng`) uses it. In most code, avoid it — return codes + goto cleanup is more maintainable.

## Pattern 6: Error context struct

For functions that need to report detailed error information beyond a simple code:

```c
typedef struct {
    int code;
    int line;                // source line where error occurred
    const char *file;        // source file
    char message[256];       // human-readable description
} Error;

#define ERROR_SET(err, c, fmt, ...) do {               \
    (err)->code = (c);                                  \
    (err)->line = __LINE__;                              \
    (err)->file = __FILE__;                              \
    snprintf((err)->message, sizeof((err)->message),     \
             fmt, ##__VA_ARGS__);                        \
} while(0)

int parse_sensor_data(const uint8_t *raw, size_t len, SensorData *out, Error *err) {
    if (len < 4) {
        ERROR_SET(err, ERR_PARSE_FAIL, "need 4 bytes, got %zu", len);
        return -1;
    }
    // ...
    return 0;
}

// caller:
Error err = {0};
if (parse_sensor_data(buf, n, &data, &err) != 0) {
    fprintf(stderr, "%s:%d: %s\n", err.file, err.line, err.message);
}
```

**Where:** OpenSSL error queue, Vulkan validation layers. Useful in larger embedded projects where a simple error code isn't enough context.

---

## Idioms

### Guard clause — early return on bad input

**What:** Check preconditions at the top of the function, return immediately on failure. Happy path continues below with no nesting.
**Why:** Avoids deeply nested `if/else` pyramids. Each check is a flat, readable line.
**When:** Every function that receives external input or pointers that could be null.
**Where:** Everywhere. The single most common error handling pattern in C.

```c
int uart_send(UART *dev, const uint8_t *data, size_t len) {
    if (!dev)  return ERR_NULL_ARG;
    if (!data) return ERR_NULL_ARG;
    if (len == 0) return ERR_OK;         // nothing to send is not an error
    if (!dev->initialized) return ERR_NOT_INIT;

    // happy path — all checks passed
    for (size_t i = 0; i < len; i++)
        uart_write_byte(dev, data[i]);
    return ERR_OK;
}
```

### Check every allocation

**What:** Every `malloc`/`calloc`/`realloc` call gets an `if (!ptr)` check immediately after.
**Why:** `malloc` returns `NULL` when out of memory. Dereferencing `NULL` is instant crash (or worse, silent corruption on embedded systems without MMU).
**When:** Every single time. No exceptions.

```c
char *buf = malloc(size);
if (!buf) {
    // on PC: log and exit, or return error
    // on embedded: assert, blink error LED, or enter safe state
    return ERR_NO_MEMORY;
}
```

### Reverse-order cleanup with `goto`

**What:** Acquire resources top-to-bottom, clean up bottom-to-top via `goto` labels.
**Why:** Ensures every error path cleans up exactly the resources that were successfully acquired.
**When:** Any function that acquires 2+ resources that need cleanup.

```c
int connect_and_query(const char *host, const char *query, Result *out) {
    int ret = ERR_FAIL;

    Socket *sock = socket_connect(host);
    if (!sock) goto out;

    Buffer *buf = buffer_create(4096);
    if (!buf) goto close_sock;

    if (socket_send(sock, query, strlen(query)) < 0)
        goto free_buf;

    if (socket_recv(sock, buf) < 0)
        goto free_buf;

    parse_result(buf, out);
    ret = ERR_OK;

free_buf:
    buffer_destroy(buf);
close_sock:
    socket_close(sock);
out:
    return ret;
}
```
