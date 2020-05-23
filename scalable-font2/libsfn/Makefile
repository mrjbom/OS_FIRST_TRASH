include Makefile.env

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(OBJS)

unicode.h: Blocks.txt UnicodeData.txt
	php unicode.php

%: %.c unicode.h
	$(CC) $(CFLAGS) $< -c $@

libsfn.a: $(OBJS)
	@rm $@ 2>/dev/null || true
	@ar -frsv $@ $^

clean:
	@rm *.a *.txt *.o 2>/dev/null || true
