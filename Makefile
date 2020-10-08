#fno-use-cxa-atexit : no memory management
#nostdlib : no glibc
#fno-leading-underscore : for naming of section
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o interruptstubs.o interrupts.o kernel.o

%.o: %.cpp
		g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
		as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
		ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
		sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
		mkdir -p isodir/boot/grub
		mkdir -p isodir/boot/grub
		cp mykernel.bin isodir/boot/mykernel.bin
		cp grub.cfg isodir/boot/grub/grub.cfg
		grub-mkrescue --output=$@ isodir
		rm -rf isodir

runn: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'My Operating System' &

execute:
		make kernel.o
		make loader.o
		make mykernel.bin
		make mykernel.iso
		qemu-system-i386 -cdrom mykernel.iso

run: mykernel.iso
	qemu-system-i386 -cdrom mykernel.iso


.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso