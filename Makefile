CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
LLVMPATH = /usr/local/opt/llvm/bin
CLANGFLAGS = -Wall -O2 -ffreestanding  -nostdlib -mcpu=cortex-a72+nosimd


# For running on the QEMU Emulator
QEMUPATH = /usr/local/Cellar/qemu/7.1.0/bin
QMACHINE = raspi3b


all: clean kernel8.img

boot.o: boot.S
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c boot.S -o boot.o

%.o: %.c
	$(LLVMPATH)/clang --target=aarch64-elf $(CLANGFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(LLVMPATH)/ld.lld -m aarch64elf -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(LLVMPATH)/llvm-objcopy -O binary kernel8.elf kernel8.img

clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true

run: all
	$(QEMUPATH)/qemu-system-aarch64 -M $(QMACHINE) -serial stdio -kernel kernel8.img 
