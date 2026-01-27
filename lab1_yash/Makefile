CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lreadline -lncurses

SRCS = yash.c parse_input.c child_process.c signal_handler.c
OBJS = $(SRCS:.c=.o)
TARGET = yash

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit header dependencies
yash.o: yash.c parse_input.h child_process.h signal_handler.h
parse_input.o: parse_input.c parse_input.h
child_process.o: child_process.c child_process.h
signal_handler.o: signal_handler.c signal_handler.h

clean:
	rm -f $(OBJS) $(TARGET)
