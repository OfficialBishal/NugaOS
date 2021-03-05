#fno-use-cxa-atexit : no memory management
#nostdlib : no glibc
#fno-leading-underscore : for naming of section

# sudo apt-get install g++ binutils libc6-dev-i386
# sudo apt-get install VirtualBox grub-legacy xorriso

GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
          obj/gdt.o \
          obj/memorymanagement.o \
          obj/drivers/driver.o \
          obj/hardwarecommunication/port.o \
          obj/hardwarecommunication/interruptstubs.o \
          obj/hardwarecommunication/interrupts.o \
          obj/multitasking.o \
          obj/drivers/amd_am79c973.o \
          obj/hardwarecommunication/pci.o \
          obj/drivers/keyboard.o \
          obj/drivers/mouse.o \
          obj/drivers/vga.o \
          obj/gui/widget.o \
          obj/gui/window.o \
          obj/gui/desktop.o \
          obj/kernel.o


run: mykernel.iso
	qemu-system-i386 -cdrom mykernel.iso

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)


mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso


install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

.PHONY: clean
clean:
	rm -rf obj mykernel.bin mykernel.iso
