CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O2
GCCPATH = ../arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-aarch64-none-elf/bin

all: clean out.exe

%.o: %.c
	gcc $(GCCFLAGS) -c $< 

out.exe: $(OFILES)
	gcc -o out.exe $(OFILES)

clean:
	del *.o