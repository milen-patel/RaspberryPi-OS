# Don't use normal gcc, use the arm cross compiler
CC = /usr/local/opt/llvm/bin/clang
LLVMPATH = /usr/local/opt/llvm/bin

CPU = cortex-a72+nosimd

CFLAGS= -mcpu=$(CPU) -fpic -ffreestanding 
CSRCFLAGS= -O2 -Wall -Wextra
LFLAGS= -ffreestanding -O2 -nostdlib

# Location of the files
KER_SRC = ../src/kernel
KER_HEAD = ../include
COMMON_SRC = ../src/common
OBJ_DIR = objects
KERSOURCES = $(wildcard $(KER_SRC)/*.c)
COMMONSOURCES = $(wildcard $(COMMON_SRC)/*.c)
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(COMMON_SRC)/%.c, $(OBJ_DIR)/%.o, $(COMMONSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))
HEADERS = $(wildcard $(KER_HEAD)/kernel/*.h)

IMG_NAME=kernel8.img

# For running on the QEMU Emulator
QEMUPATH = /usr/local/Cellar/qemu/7.1.0/bin
QMACHINE = raspi3b


build: $(OBJECTS) $(HEADERS)
	$(LLVMPATH)/ld.lld -m aarch64elf -nostdlib $(OBJECTS) -T link.ld -o kernel8.elf
	$(LLVMPATH)/llvm-objcopy -O binary kernel8.elf kernel8.img

$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
	mkdir -p $(@D)
	$(CC) --target=aarch64-elf $(CFLAGS) -I$(KER_SRC) -I ../include/common -I ../include/kernel -c $< -o $@ $(CSRCFLAGS)

$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	mkdir -p $(@D)
	$(CC) --target=aarch64-elf $(CFLAGS) -I$(KER_SRC) -c $< -o $@

$(OBJ_DIR)/%.o: $(COMMON_SRC)/%.c
	mkdir -p $(@D)
	$(CC) --target=aarch64-elf $(CFLAGS) -I$(KER_SRC) -I ../include/common -I ../include/kernel -c $< -o $@ $(CSRCFLAGS)

clean:
	rm -rf $(OBJ_DIR)
	rm $(IMG_NAME)

run: build
	$(QEMUPATH)/qemu-system-aarch64 -M $(QMACHINE) -serial stdio -kernel kernel8.img 
