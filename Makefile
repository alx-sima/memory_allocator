CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
TARGET=vma

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm -f $(TARGET) $(OBJS) vgcore.*