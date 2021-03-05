/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    (ALIGN | MEMINFO)  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernelMain
.extern callConstructors
.global loader


loader:
    mov $kernel_stack, %esp
    call callConstructors   #test
    push %eax   #use info from multiboot structure
    push %ebx   #use info from multiboot structure
    call kernelMain


_stop:
    cli     # clear interrupt
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024; # 2 MiB = 2B x KB x MB
kernel_stack:

