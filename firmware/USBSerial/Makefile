CC = msp430-gcc
CXX = msp430-ar
CFLAGS = -O2 -Wall -mmcu=msp430f5529

SRCS=$(wildcard *.c *.cpp)
OBJS=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

F_CPU=16000000
CFLAGS+=-I. -DF_CPU=$(F_CPU)

all: $(OBJS)

.PHONY: all clean install mspdebug debug gdb tui ddd

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr $(OBJS)
