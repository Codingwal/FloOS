CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
GCCPATH = arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-aarch64-none-elf/bin

all: clean kernel8.img

boot.o: boot.s
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c boot.s -o boot.o

%.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	del kernel8.elf *.o *.img