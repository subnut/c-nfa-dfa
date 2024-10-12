.POSIX:
all: main

# CFLAGS = -g -ggdb
CFLAGS = -g -ggdb -O3 -fno-omit-frame-pointer -std=c99 $(SANITIZE)
SANITIZE = -fsanitize=address

main: main.c \
	nfa.c dfa.c nfa2dfa.c \
	nfa.h dfa.h nfa2dfa.h \
	common.h macros.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c nfa.c dfa.c nfa2dfa.c
