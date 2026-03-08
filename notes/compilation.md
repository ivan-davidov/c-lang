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
