CFILES = $(wildcard src/*.c)
OFILES = $(CFILES:.c=.o)
GCCPATH = arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-aarch64-none-elf/bin

GCCFLAGS-OS = -ffreestanding -nostdinc -nostdlib -nostartfiles -DOS=1
GCCFLAGS = -Iinclude -Wall -O2


# Compile as app

app: out.exe

out.exe: $(OFILES)
	gcc -o out.exe $(OFILES)


# Compile as OS

os: kernel8.img

boot.o: asm/boot.s
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(GCCFLAGS-OS) -c asm/boot.s -o boot.o

ifeq ($(MAKECMDGOALS), os)
%.o: %.c
		$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(GCCFLAGS-OS) -c $< -o $@

else
%.o: %.c
	gcc $(GCCFLAGS) -c $< -o $@
endif

kernel8.img: boot.o $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	del /S *.o
	del kernel8.elf kernel8.img out.exe