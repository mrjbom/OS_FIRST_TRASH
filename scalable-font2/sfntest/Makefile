SRCS = $(wildcard *.c)
BINS = $(SRCS:.c=)
CFLAGS = -ansi -pedantic -Wall -Wextra -I/usr/include/SDL2 -I../ -g
LIBS = -lSDL2
ifneq ("$(wildcard /usr/include/zlib.h)","")
CFLAGS += -DHAS_ZLIB=1
LIBS += -lz
endif

all: $(BINS) size

testC++: sfntest2.c
	g++ $(CFLAGS:-ansi=-std=c++11) $< -o $@ $(LIBS)

%: %.c ../ssfn.h
	gcc $(CFLAGS) $< -o $@ $(LIBS)

size: sfntest1 sfntest2
	@./size.sh

clean:
	@rm $(BINS) testC++ *.o 2>/dev/null || true
