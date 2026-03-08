# Shared Makefile for chapter directories
# Each .c file compiles to its own binary (ex01.c → ex01)
#
# Usage: create a chapter Makefile with just:
#   include ../../Makefile.inc
# (adjust ../ depth to match directory level)

CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c17 -g
LDFLAGS  =
LDLIBS   =

SRCS     = $(wildcard *.c)
TARGETS  = $(SRCS:.c=)

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

debug: CFLAGS += -fsanitize=address,undefined -O0 -DDEBUG
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean all

clean:
	rm -f $(TARGETS) *.o *.d

.PHONY: all clean debug
