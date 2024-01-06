CC=g++
CPPFLAGS= -Wall -Wextra -O2 -ffreestanding -I bootsrc -I limine-efi/inc -I limine-efi/inc/x86_64

LD= limine-efi/gnuefi/elf_x86_64_efi.lds

LDFLAGS= -nostdlib -static -pie --no-dynamic-linker -T $(LD)

CPPFILES:=$(shell cd bootsrc && find -L * -type f -name '*.cpp')
OBJ:=$(addprefix obj/,$(CPPFILES:.cpp=.cpp.o))
HEADER_DEPS:=$(addprefix obj/,$(CPPFILES:.cpp=.cpp.d))

SRC=bootsrc

.PHONY: all

all: always AquaBoot.EFI

limine-efi/gnuefi/crt0-efi-x86_64.S: limine-efi

limine-efi/gnuefi/crt0-efi-x86_64.S.o: limine-efi/gnuefi/crt0-efi-x86_64.S
	$(MAKE) -C limine-efi/gnuefi crt0-efi-x86_64.S.o

limine-efi/gnuefi/reloc_x86_64.c: limine-efi

limine-efi/gnuefi/reloc_x86_64.c.o: limine-efi/gnuefi/reloc_x86_64.c
	$(MAKE) -C limine-efi/gnuefi reloc_x86_64.c.o

limine-efi/gnuefi/elf_x86_64_efi.lds: limine-efi

limine-efi:
	git clone https://github.com/limine-bootloader/limine-efi.git

AquaBoot.EFI: AquaBoot.elf Makefile
	objcopy -O binary bin/$< bin/$@

AquaBoot.elf: Makefile limine-efi/gnuefi/elf_x86_64_efi.lds limine-efi/gnuefi/crt0-efi-x86_64.S.o limine-efi/gnuefi/reloc_x86_64.c.o $(OBJ)
	echo $(CPPFILES)
	ld limine-efi/gnuefi/crt0-efi-x86_64.S.o limine-efi/gnuefi/reloc_x86_64.c.o $(OBJ) $(LDFLAGS) -o bin/$@

-include $(HEADER_DEPS)

obj/%.cpp.o: bootsrc/%.cpp Makefile limine-efi
	$(CC) $(CPPFLAGS) -c $< -o $@

always:
	mkdir -p bin
	mkdir -p obj

 .PHONY: clean

clean:
	rm -rf limine-efi
	rm -rf bin
	rm -rf obj
