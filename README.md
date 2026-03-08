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

## Rules

- Read a chapter, write code the same day
- Commit everything, even if it's broken
- 80% coding, 20% reading — every week
