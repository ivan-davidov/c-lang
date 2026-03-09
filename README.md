# c-lang

Book exercises and examples while learning C. Three books, read in parallel
as the roadmap dictates — not cover to cover.

## Books

- **King** — *C Programming: A Modern Approach* — primary, chapters 1-20
- **Modern C** — Gustedt — reference when King is unclear
- **Effective C** — Seacord — secure C, undefined behavior, safety (from Module 4)

## Structure

```
c-lang/
├── king/
│   ├── ch01-03/          # Module 1: program structure, variables, types, printf
│   ├── ch04-06/          # Module 2: if/else, loops, functions
│   ├── ch11/             # Module 3a: pointers
│   ├── ch12-13/          # Module 3b: arrays and strings
│   └── ch15-16/          # Module 4: structs, pointers to structs
├── modern-c/
│   ├── ch01/             # Module 1: backup reference
│   └── ch06/             # Module 3a: pointers reference
├── effective-c/
│   └── ch01-03/          # Module 4: UB, object types, safety
└── snippets/             # reusable patterns discovered along the way
```

Each chapter directory holds exercise files (`ex01.c`, `ex02.c`, ...) and
an optional `notes.txt` for gotchas and things that surprised you.

## Build

Every `.c` file is a standalone program. Compile from the chapter directory:

```bash
cd king/ch01-03
make              # builds all exercises
make ex01         # build one
make debug        # ASan + UBSan
make clean
```

## Reading list

### C language

| Book | What it's for | When |
|------|--------------|------|
| **C Programming: A Modern Approach** — King | Primary C textbook, clear, thorough | First, cover to cover |
| **Modern C** — Gustedt | Reference when King is unclear, more modern perspective | JIT alongside King |
| **Effective C** — Seacord | Secure C, undefined behavior, defensive coding | After King Ch. 16 |
| **Expert C Programming** — van der Linden | Deep corners of C: declarations, linkage, memory layout, war stories | After finishing King |
| **The C Puzzle Book** — Feuer | Tricky expression evaluation, tests your understanding | Quick read anytime |

### Systems programming

| Book | What it's for | When |
|------|--------------|------|
| **CS:APP** — Bryant & O'Hallaron | Memory, assembly, processes, concurrency, networking — the foundation | Phase 4+ (labs are the real value) |
| **OSTEP** — Arpaci-Dusseau | OS internals: processes, threads, scheduling, virtual memory (free online) | Phase 4, before/alongside FreeRTOS |
| **The Linux Programming Interface** — Kerrisk | Definitive POSIX/Linux API reference: files, processes, signals, sockets, IPC | Reference book, not cover to cover |
| **Unix Network Programming** — Stevens | Sockets, TCP/UDP, select/poll/epoll, the real depth | When you need networking beyond Beej |

### Hands-on project resources (free)

| Resource | What you build | When |
|----------|---------------|------|
| **Beej's Guide to Network Programming** | Sockets, client/server, multiplexing | After CS:APP Ch. 11 |
| **Beej's Guide to C** | Quick C reference, good examples | Anytime as reference |
| **Build Your Own Lisp** — Holden | Lisp interpreter in C: parsing, tagged unions, trees, malloc | After King Ch. 16 |
| **Crafting Interpreters** (Part III) — Nystrom | Bytecode VM in C: hash tables, GC, compiler | After Build Your Own Lisp |
| **CS:APP Labs** (CMU) | Shell, malloc, proxy, bomb, attack | Alongside CS:APP chapters |
| **Stephen Brennan's shell tutorial** | Minimal Unix shell (~200 lines) | After CS:APP Ch. 8 |

### Embedded (from the roadmap)

| Book | What it's for | When |
|------|--------------|------|
| **Beginning STM32** — Gay | Bridge from C to STM32 hardware | Phase 2 |
| **Making Embedded Systems** — White | Embedded architecture, patterns, debugging | Phase 5+ |
| **Test Driven Development for Embedded C** — Grenning | Testing firmware | When projects get complex |

### Not recommended

| Book | Why skip |
|------|---------|
| **Understanding and Using C Pointers** — Reese | Disorganized, has errors, King Ch. 11-12 + practice is better |
| **C Programming Language** (K&R) | Historic but outdated (C89), terse to the point of unhelpful, better books exist now |
| **Head First C** | Too shallow, won't add anything after King |

## Rules

- Read a chapter, write code the same day
- Commit everything, even if it's broken
- 80% coding, 20% reading — every week
