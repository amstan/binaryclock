# Makefile created by gnomad

PROG = main
CC = msp430-gcc
CXX = msp430-g++
OBJDUMP = msp430-objdump
SIZE = msp430-size
MSPDEBUG = mspdebug
CFLAGS = -Os -Wall -mmcu=msp430f5529 -I./USBSerial/
FET = tilib
GDB = msp430-gdb
GDBTUI = $(GDB)tui

OBJS=ws2811.o $(PROG).o

all: $(PROG).elf  $(PROG).lst
	#cd USBSerial; make
	$(SIZE) $(PROG).elf

.PHONY: all clean install mspdebug debug gdb tui ddd

$(PROG).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(wildcard USBSerial/*.o) -o $(PROG).elf

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.S
	$(CC) $(CFLAGS) -c -x assembler-with-cpp $<

%.lst: %.elf
	$(OBJDUMP) -DS $< >$@

clean:
	rm -fr $(PROG).elf $(PROG).lst $(OBJS)

install: $(PROG).elf
	$(MSPDEBUG) $(FET) "prog $(PROG).elf"

mspdebug: $(PROG).elf
	$(MSPDEBUG) $(FET)

debug: $(PROG).elf
	$(MSPDEBUG) $(FET) gdb

gdb: $(PROG).elf
	$(GDB) $(PROG).elf
 
tui: $(PROG).elf
	$(GDBTUI) $(PROG).elf
 
ddd: $(PROG).elf
	ddd --debugger $(GDB) $(PROG).elf