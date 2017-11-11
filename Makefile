CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11

TARGETS = gol

all: $(TARGETS)

gol: gol.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGETS)

