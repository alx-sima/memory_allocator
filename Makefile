# Copyright: Sima Alexandru (312CA) 2023

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
TARGET=vma
ARCHIVE=$(TARGET).zip

.PHONY: build run_vma pack clean

build: $(TARGET)

run_vma: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

pack:
	zip -FSr $(ARCHIVE) README.md Makefile *.c *.h

clean:
	-rm -f $(TARGET) $(OBJS) $(ARCHIVE) vgcore.*
