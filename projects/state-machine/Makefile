CC       = gcc
CFLAGS   = -Wall -Wextra -Wpedantic -std=c17 -g
LDFLAGS  =
LDLIBS   =

SRCS     = $(wildcard *.c)
OBJS     = $(SRCS:.c=.o)
TARGET   = fsm_demo

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

debug: CFLAGS += -fsanitize=address,undefined -O0 -DDEBUG
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean all

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean debug run
