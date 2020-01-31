# Copyright 2020 Charles Daniels, Jason Bakos, Philip Conrad

include config.mk

# this defines the C macros F_CPU and UART_BAUDRATE
F_CPU_DEF=-DF_CPU=$(F_CPU)
UART_BAUDRATE_DEF=-DUART_BAUDRATE=$(UART_BAUDRATE)
DEFS=$(F_CPU_DEF) $(UART_BAUDRATE_DEF)

CC = avr-gcc
CFLAGS = $(OPTIMIZATION) -mmcu=$(MCUNAME) $(DEFS) -std=c99

# username to pack under
USER?=$(shell whoami)

# name for the tarball to generate on make pack
TARBALL=$(shell echo "$(USER)_CSCE317_2020SP_lab_uart")

# regex to filter files that should be included in submission
INCLUDEFILTER=(.*.c$$)|(.*.h$$)|(.*.mk$$)|(Makefile)|(reflection.txt)

# list of files that should be included
INCLUDEFILES=$(shell find . -not -path './handout/*' -print | awk '/$(INCLUDEFILTER)/')

.PHONY: clean all burn pack reference

all: main.elf main.lst main.hex main.map

main.elf: main.o  # add more objects HERE
	$(CC) $(CFLAGS) -o main.elf $^

# generate an assembler dump from the C code
main.lst: main.elf
	avr-objdump -C -g -d -h -S main.elf > main.lst

# generate the memory map of the ELF file
main.map: main.elf
	$(CC) $(CFLAGS) -Wl,-Map,main.map *.o

# generate an avrdude-compatible hex file
main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# generic rules to generate object files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

# flash the AVR with the generate hex file
burn: main.hex
	@printf "Burn Baby Burn...\n"
	avrdude -p $(PARTNO) -c $(AVRDUDE_PROGRAMMER) -U flash:w:main.hex:i

# Flash the AVR with the reference solution. The .dat is just so it dosen't
# get deleted by make clean.
reference:
	avrdude -p $(PARTNO) -c $(AVRDUDE_PROGRAMMER) -U flash:w:reference.dat:i

pack: $(TARBALL).tgz

# create submission tarball
$(TARBALL).tgz: $(INCLUDEFILES)
	@if [ "$$(shasum reflection.txt)" = "3fb96cae95d50726188deaf0aefde12ba0ed66e2  reflection.txt" ] ; then echo "oops! It looks like you didn't fill in reflection.txt!" 1>&2 ; exit 1 ; fi
	rm -f $(TARBALL).tgz
	tar cvfz $@ --transform 's,^,$(TARBALL)/,' $^


clean:
	rm -f *.o *.lst *.elf *.map *.hex a.out $(TARBALL)

