# Compilation

- C compiles to native machine code — no runtime, no VM, no GC
- compilation pipeline: preprocessor → compiler → assembler → linker
- `-Wall -Wextra -Wpedantic` — always, catches real bugs that compile silently
- `-std=c17` — pin the standard, don't rely on compiler defaults
- `-g` — debug symbols, needed for gdb and valgrind to show line numbers
- `-fsanitize=address,undefined` — catches memory errors and UB at runtime,
  use during development, never ship with it
- each `.c` file compiles independently into a `.o` — linker combines them
- `#include` literally pastes the file contents — it's text substitution

## Common invocations

```bash
gcc -o prog main.c              # compile + link in one step
gcc -c main.c                   # compile only → main.o (no linking)
gcc -o prog main.o util.o       # link multiple .o files

gcc -E main.c                   # preprocessor output only — see what #include/macros expand to
gcc -S main.c                   # emit assembly → main.s (read with less or your editor)
gcc -S -O2 -fno-asynchronous-unwind-tables main.c  # cleaner asm output

gcc -Wall -Wextra -Wpedantic -Wconversion  # the full paranoia set
gcc -Werror                     # treat warnings as errors — strict but useful in CI
gcc -v main.c                   # verbose — shows full pipeline (preprocessor, cc1, as, ld)
```

## Multi-file compilation idiom

```bash
# development: compile changed files only, link all
gcc -c main.c                   # only recompile what changed
gcc -c util.c
gcc -o prog main.o util.o       # link step

# this is exactly what make does — tracks dependencies, rebuilds minimum
```
