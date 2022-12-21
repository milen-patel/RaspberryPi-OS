# Don't use normal gcc, use the arm cross compiler
COMPILER-PATH = /usr/local/Cellar/aarch64-unknown-linux-gnu/11.2.0/bin/aarch64-unknown-linux-gnu

# Flags to be passed to gcc 
C-COMPILER-FLAGS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASM-COMPILER-FLAGS = -Iinclude

# Location of the files
OUTPUT-DIRECTORY = build
SOURCE-DIRECTORY = src

IMG_NAME=kernel8.img

# For running on the QEMU Emulator
QEMUPATH = /usr/local/Cellar/qemu/7.1.0/bin
QMACHINE = raspi3b

# Gather all the source files
C-FILES = $(wildcard $(SOURCE-DIRECTORY)/common/*.c)
C-FILES += $(wildcard $(SOURCE-DIRECTORY)/kernel/*.c)
ASM-FILES = $(wildcard $(SOURCE-DIRECTORY)/common/*.s)
ASM-FILES += $(wildcard $(SOURCE-DIRECTORY)/kernel/*.s)

# Compile the source files
OBJECT-FILES = $(C-FILES:$(SOURCE-DIRECTORY)/%.c=$(OUTPUT-DIRECTORY)/%_c.o)
OBJECT-FILES += $(ASM-FILES:$(SOURCE-DIRECTORY)/%.s=$(OUTPUT-DIRECTORY)/%_s.o)

all: kernel

# By default, the goal is the first target in the make file

$(OUTPUT-DIRECTORY)/%_c.o: $(SOURCE-DIRECTORY)/%.c
	mkdir -p $(@D)
	$(COMPILER-PATH)-gcc $(C-COMPILER-FLAGS)  -c $< -o $@ 

$(OUTPUT-DIRECTORY)/%_s.o: $(SOURCE-DIRECTORY)/%.s
	mkdir -p $(@D)
	$(COMPILER-PATH)-gcc $(ASM-COMPILER-FLAGS) -c $< -o $@ 

clean:
	rm -rf $(OUTPUT-DIRECTORY)
	rm *.img

run: build
	$(QEMUPATH)/qemu-system-aarch64 -M $(QMACHINE) -serial stdio -kernel kernel8.img 

kernel: $(OBJECT-FILES)
	$(info    C FILES is $(C-FILES))
	$(info    ASM FILES is $(ASM-FILES))
	$(info    OBJECT FILES is $(OBJECT-FILES))
	$(COMPILER-PATH)-ld -m aarch64elf -nostdlib $(OBJECT-FILES) -T $(SOURCE-DIRECTORY)/kernel/link.ld -o $(OUTPUT-DIRECTORY)/kernel8.elf
	$(COMPILER-PATH)-objcopy -O binary $(OUTPUT-DIRECTORY)/kernel8.elf kernel8.img
	echo "LMAO"