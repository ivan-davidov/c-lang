# c-lang

C projects from the embedded learning roadmap.
Tracking and reading lists live in `org/learning/c-embedded.org`.

## Structure

```
c-lang/
├── projects/              # Roadmap projects
│   ├── state-machine/     # Stage 1: FSM framework
│   ├── ring-buffer/       # Stage 2: circular buffer library
│   ├── uart-parser/       # Stage 3: command parser
│   ├── tcp-client/        # Stage 4: socket client
│   ├── my-malloc/         # Stage 4: custom allocator
│   ├── producer-consumer/ # Stage 5: pthreads + mutex
│   └── mini-shell/        # Stage 5: fork/exec/pipe
├── snippets/              # Reusable patterns
└── Makefile.inc           # Shared build rules
```

## Build

Each project has its own Makefile. For multi-file projects:

```bash
cd projects/state-machine
make              # build
make debug        # ASan + UBSan
make clean
```

## Rules

- Commit everything, even if it's broken
- 80% coding, 20% reading — every week
