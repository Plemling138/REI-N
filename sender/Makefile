PREFIX	= /usr/h8
ARCH	= h8300-elf
BINDIR	= $(PREFIX)/bin
ADDNAME	= $(ARCH)-

#Use tools select
AR	= $(BINDIR)/$(ADDNAME)ar
AS	= $(BINDIR)/$(ADDNAME)as
CC	= $(BINDIR)/$(ADDNAME)gcc
LD	= $(BINDIR)/$(ADDNAME)ld
NM	= $(BINDIR)/$(ADDNAME)nm
OBJCOPY	= $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP	= $(BINDIR)/$(ADDNAME)objdump
RANLIB	= $(BINDIR)/$(ADDNAME)ranlib
STRIP	= $(BINDIR)/$(ADDNAME)strip

#do compile sourcecode
OBJS	= startup.o main.o intr.o interrupt.o vector.o dram.o
OBJS	+= lib.o serial.o ether.o lcd.o

#SystemSource
OBJS	+= rei.o syscall.o memory.o command.o recv.o send.o timer.o usertask.o
OBJS  += consdrv.o ethdrv.o

TARGET	= rei

CFLAGS	= -Wall -mh -nostdinc -nostdlib -fno-builtin
#CFLAGS	+= -mint32 
CFLAGS	+= -I.
#CFLAGS	+= -g
CFLAGS	+= -Os
CFLAGS	+= -DSYSTEM

LFLAGS	= -static -T ld.scr -L.

.SUFFIXES:	.c .o
.SUFFIXES:	.s .o
.SUFFIXES:	.S .o

all :	$(TARGET)

$(TARGET) :	$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS) $(LFLAGS)
	cp $(TARGET) $(TARGET).elf
	$(STRIP) $(TARGET)

.c.o :	$<
	$(CC) -c $(CFLAGS) $<

.s.o :	$<
	$(CC) -c $(CFLAGS) $<

.S.o :	$<
	$(CC) -c $(CFLAGS) $<


$(TARGET).mot : $(TARGET)
	$(OBJCOPY) -O srec $(TARGET) $(TARGET).mot

image:	$(TARGET).mot


clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).elf
