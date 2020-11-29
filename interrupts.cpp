
#include "interrupts.h"


void printf(char* str);


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

// set entries in the IDT
void InterruptManager::SetInterruptDescriptorTableEntry(
		uint8_t interruptNumber,
		uint16_t codeSegmentSelectorOffset,
		void (*handler)(),
		uint8_t DescriptorPrivilegeLevel,
		uint8_t DescriptorType)
{
	// address of pointer to code segment (relative to global descriptor table)
    // and address of the handler (relative to segment)
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t) handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t) handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;

    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
    interruptDescriptorTable[interruptNumber].reserved = 0;		
}

// constructor
InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
: picMasterCommand(0x20),	//instantiate port_no for interrupt
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)
{
	uint16_t CodeSegment = gdt->CodeSegmentSelector();	// get offset of CodeSegment
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	// firstly, set entries to ignoreInterrupt
	for(uint16_t i = 0; i < 256; i++){
		SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
	}

	//explicitely define entry in IDT
	// 0x00 + 0x20(IDT_BASE) = 0x20
	// go to assembly code
	SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
	SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);


	// communicate with PIC
	picMasterCommand.Write(0x11);
	picSlaveCommand.Write(0x11);

	picMasterData.Write(0x20);	// telling to add 0x20 to interrupt since 0x01 is used for exception by CPU too.
	picSlaveData.Write(0x28);

	picMasterData.Write(0x04);	// tell master, it is master
	picSlaveData.Write(0x02);	// tell slave, it is slave

	picMasterData.Write(0x01);
	picSlaveData.Write(0x01);

	picMasterData.Write(0x00);
	picSlaveData.Write(0x00);


	// tell processor to use IDT
	InterruptDescriptorTablePointer idt;
	idt.size = 256 * sizeof(GateDescriptor) - 1;
	idt.base = (uint32_t)interruptDescriptorTable;
	asm volatile("lidt %0" : : "m" (idt));
}



InterruptManager::~InterruptManager()
{
}

// start the interrupt after being loaded into processor
void InterruptManager::Activate()
{
	asm("sti");
}


uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{

	printf(" INTERRUPT");


	return esp;
}
