#reference from tynu project
.set IRQ_BASE, 0x20

.section .text

# name of this 'handle interrupt' called outside
# make interrupts.o
# nm interrupts.o
.extern _ZN16InterruptManager15HandleInterruptEhj

.global _ZN16InterruptManager22IgnoreInterruptRequestEv


.macro HandleException num
.global _ZN16InterruptManager16HandleException\num\()Ev
_ZN16InterruptManager16handleException\num\()Ev:
	movb $\num, (interruptnumber)
	jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
	movb $\num + IRQ_BASE, (interruptnumber)
	jmp int_bottom
.endm



HandleInterruptRequest 0x00
HandleInterruptRequest 0x01


int_bottom:
	
# save the current registers value
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

# jump into handleInterrupt fucntion
	push %esp
	push (interruptnumber)
	call _ZN16InterruptManager15handleInterruptEhj
	# addl $5, %esp		// clean these from stack 
	movl %eax, %esp

# retrieve the saved registers value
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

# Ignore Interrupt
# tell processor to return to original process
_ZN16InterruptManager22IgnoreInterruptRequestEv:
	iret


.data
	interruptnumber: .byte 0
