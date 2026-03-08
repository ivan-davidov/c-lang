# Number Systems — Binary, Hex, Octal

## The core idea

Decimal, binary, hex, octal are **the same numbers** — just different ways to write them down.
You don't "convert" 42 to binary. 42 *is* `00101010`. Always was. You're just changing the lens.

## Why multiple bases exist

| Base | Why it exists | Where you'll see it |
|------|--------------|---------------------|
| Decimal (10) | Human habit — 10 fingers | printf output, user-facing values |
| Binary (2) | How hardware thinks — voltage on or off | register bits, flags, masks |
| Hex (16) | Compact way to read binary — each hex digit = exactly 4 bits | memory addresses, register values, color codes |
| Octal (8) | Unix file permissions, legacy — each digit = exactly 3 bits | `chmod 755`, C string escapes (`\033`) |

## Binary — the machine's language

Each digit (bit) is a power of 2, right to left:

```
bit position:   7    6    5    4    3    2    1    0
power of 2:   128   64   32   16    8    4    2    1

example: 42
          0    0    1    0    1    0    1    0
               ×    ×              ×         ×
              64   32    16    8    4    2    1
                   32    +    8    +    2    = 42
```

The intuition: each bit is a yes/no answer to "do I include this power of 2?"

```
42 = 32 + 8 + 2
   = 2⁵ + 2³ + 2¹
   = 00101010
```

### Powers of 2 — memorize these, they're everywhere

```
2⁰  = 1        2⁴  = 16       2⁸  = 256       2¹² = 4096
2¹  = 2        2⁵  = 32       2⁹  = 512       2¹⁶ = 65536
2²  = 4        2⁶  = 64       2¹⁰ = 1024      2²⁰ = 1048576 (1M)
2³  = 8        2⁷  = 128      2¹¹ = 2048       2³² = 4294967296 (4G)
```

### Byte boundaries matter

```
 8 bits = 1 byte   = uint8_t   = 0..255
16 bits = 2 bytes  = uint16_t  = 0..65535
32 bits = 4 bytes  = uint32_t  = 0..4,294,967,295
64 bits = 8 bytes  = uint64_t  = 0..18.4 quintillion
```

## Hex — the human-readable binary

Hex exists because binary is unreadable in bulk. The trick: **every hex digit maps to exactly 4 bits**.

```
hex:    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
bin: 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
dec:    0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
```

To read hex as binary, expand each digit to 4 bits. That's it. No math needed:

```
0x2A = 0010 1010 = 42
       ──── ────
        2    A

0xDEAD = 1101 1110 1010 1101
         ──── ──── ──── ────
          D    E    A    D

0xFF = 1111 1111 = 255 = all bits set in a byte
```

### Why hex clicks for hardware

One byte = 2 hex digits. Always. Clean, fixed width:

```
uint8_t  = 0x00 .. 0xFF            (2 digits)
uint16_t = 0x0000 .. 0xFFFF        (4 digits)
uint32_t = 0x00000000 .. 0xFFFFFFFF (8 digits)
```

When you see `0x0020`, you know instantly: bit 5 is set. In decimal that's 32 — meaningless noise when you're looking at register flags.

## Octal — you'll see it, but rarely write it

Each octal digit = exactly 3 bits:

```
octal: 0  1  2  3  4  5  6  7
bin: 000 001 010 011 100 101 110 111
```

Only place it matters — Unix permissions:

```
chmod 755 = 111 101 101
            rwx r-x r-x
            ─── ─── ───
             7   5   5
           owner grp other
```

## C syntax

```c
int dec = 42;        // decimal — no prefix
int bin = 0b101010;  // binary  — 0b prefix (C23, gcc/clang extension before that)
int hex = 0x2A;      // hex     — 0x prefix
int oct = 052;       // octal   — 0 prefix (trap! leading zero = octal)

printf("%d\n", 42);  // 42      decimal
printf("%x\n", 42);  // 2a      hex lowercase
printf("%X\n", 42);  // 2A      hex uppercase
printf("%o\n", 42);  // 52      octal
// no standard printf for binary
```

**The leading zero trap:**
```c
int x = 010;  // this is 8, not 10! leading 0 means octal
int y = 0;    // this is fine — zero is zero in any base
```

## Bitwise connection — why this matters for C

Once you see numbers as bits, bit operations become obvious:

```c
0x0F        = 0000 1111    // mask: lower 4 bits (lower nibble)
0xF0        = 1111 0000    // mask: upper 4 bits (upper nibble)
1 << 5      = 0010 0000    // bit 5 set = 0x20 = 32
~0          = 1111...1111  // all bits set
(1 << n) - 1              // mask of n lowest bits: n=4 → 0x0F
```

This is the foundation for register manipulation in embedded:
```c
REG |=  (1 << 5);    // set bit 5
REG &= ~(1 << 5);    // clear bit 5
REG ^=  (1 << 5);    // toggle bit 5
if (REG & (1 << 5))  // test bit 5
```
