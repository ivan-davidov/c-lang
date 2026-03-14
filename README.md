# c-lang

Learning C, electronics, and embedded systems. See [ROADMAP.md](ROADMAP.md)
for the full plan and [IDEAS.md](IDEAS.md) for advanced project ideas.

## Books

- **King** — *C Programming: A Modern Approach* — primary, chapters 1-20
- **Modern C** — Gustedt — reference when King is unclear
- **Effective C** — Seacord — secure C, undefined behavior, safety (Stage 1)

## Structure

```
c-lang/
├── king/                  # King chapter exercises
│   ├── ch01-03/           # program structure, variables, types, printf
│   ├── ch04-06/           # if/else, loops, functions
│   ├── ch11/              # pointers
│   ├── ch12-13/           # arrays and strings
│   └── ch15-16/           # structs, pointers to structs
├── modern-c/              # Modern C reference exercises
├── effective-c/           # Effective C exercises (UB, types, safety)
├── projects/              # C projects from the roadmap
│   ├── state-machine/     # Stage 1: FSM framework
│   ├── ring-buffer/       # Stage 2: circular buffer library
│   ├── uart-parser/       # Stage 3: command parser
│   ├── tcp-client/        # Stage 4: socket client (Beej's)
│   ├── my-malloc/         # Stage 4: custom allocator
│   ├── producer-consumer/ # Stage 5: pthreads + mutex
│   └── mini-shell/        # Stage 5: fork/exec/pipe
├── notes/                 # concept notes and gotchas
└── snippets/              # reusable patterns discovered along the way
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
| **CS:APP** — Bryant & O'Hallaron | Memory, assembly, processes, concurrency, networking — the foundation | Stage 6 (labs are the real value) |
| **OSTEP** — Arpaci-Dusseau | OS internals: processes, threads, scheduling, virtual memory (free online) | Stages 5–6, before/alongside FreeRTOS |
| **The Linux Programming Interface** — Kerrisk | Definitive POSIX/Linux API reference: files, processes, signals, sockets, IPC | Reference book, not cover to cover |
| **Unix Network Programming** — Stevens | Sockets, TCP/UDP, select/poll/epoll, the real depth | When you need networking beyond Beej |

### Hands-on project resources (free)

| Resource | What you build | When |
|----------|---------------|------|
| **Beej's Guide to Network Programming** | Sockets, client/server, multiplexing | Stage 4 (TCP client project) |
| **Beej's Guide to C** | Quick C reference, good examples | Anytime as reference |
| **CS:APP Labs** (CMU) | Shell, malloc, proxy, bomb, attack | Stage 6, alongside CS:APP chapters |
| **Stephen Brennan's shell tutorial** | Reference for mini shell project (~200 lines) | Stage 5 |
| **Crafting Interpreters** (Part III) — Nystrom | Bytecode VM in C: hash tables, GC, compiler | Stage 7+ |

### Embedded (from the roadmap)

| Book | What it's for | When |
|------|--------------|------|
| **Beginning STM32** — Gay | Bridge from C to STM32 hardware | Stages 2–4 |
| **Making Embedded Systems** — White | Embedded architecture, patterns, debugging | Stage 7+ |
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
