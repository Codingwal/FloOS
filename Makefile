CFILES = $(wildcard src/*.c src/drivers/*.c)
OCFILES = $(CFILES:.c=.o)
SFILES = $(wildcard src/*.s)
OSFILES = $(SFILES:.s=.o)
GCCPATH = arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-aarch64-none-elf/bin

GCCFLAGS-OS = -ffreestanding -nostdinc -nostdlib -nostartfiles -DOS=1 -mgeneral-regs-only
GCCFLAGS = -Iinclude -Wall -O2 -Werror -Wextra 

# Create kernel8.img
os: kernel8.img

# Compile assembly files
%.o: %.s
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(GCCFLAGS-OS) -c $< -o $@

# Compile c files
%.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(GCCFLAGS-OS) -c $< -o $@

# Link files and extract .img from .elf
kernel8.img: $(OCFILES) $(OSFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib $(OCFILES) $(OSFILES) -T link.ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img


# Clean
clean:
	del /S *.o
	del kernel8.elf kernel8.img out.exe