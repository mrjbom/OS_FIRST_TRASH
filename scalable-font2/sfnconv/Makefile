TARGET = sfnconv

include ../libsfn/Makefile.env

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

libsfn:
	@make -C ../libsfn all

%: %.c
	$(CC) $(CFLAGS) $< -c $@

$(TARGET): libsfn $(OBJS)
	$(CC) ../libsfn/*.o $(OBJS) $(LIBS) -o $(TARGET)

clean:
	@make -C ../libsfn clean || true
	@rm $(TARGET) *.o 2>/dev/null || true
