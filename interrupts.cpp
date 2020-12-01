
#include "interrupts.h"


void printf(char* str);	// linker links everything
void printfHex(uint8_t);


// Interrupt Handler Section

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if(interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}




InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager = 0;	//initialize pointer to 0

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
	// uint32_t CodeSegment = globalDescriptorTable->CodeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	// firstly, set entries to ignoreInterrupt
	for(uint16_t i = 0; i < 256; i++)
	{
		handlers[i] = 0;
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
	Deactivate();
}

// start the interrupt after being loaded into processor
void InterruptManager::Activate()
{
	if(ActiveInterruptManager != 0)
		ActiveInterruptManager -> Deactivate();

	ActiveInterruptManager = this;
	asm("sti");
}

// stop the interrupt after being loaded into processor
void InterruptManager::Deactivate()
{
	if(ActiveInterruptManager == this)
	{
		ActiveInterruptManager = 0;
		asm("cli");
	}
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
	if (ActiveInterruptManager != 0)
	{
		return ActiveInterruptManager -> DoHandleInterrupt(interruptNumber, esp);
	}
	return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp)
{

	if(handlers[interruptNumber] != 0)
    {
    	printf("HANDLING INTERRUPT...");
        esp = handlers[interruptNumber]->handleInterrupt(esp);	//it was captital 'H' before
    }
	else if(interruptNumber != 0x20)	//0x20 is for timer
	{
		printf("UNHANDLED INTERRUPT 0x");
		printfHex(interruptNumber);
	}

	if(0x20 <= interruptNumber && interruptNumber < 0x30)
	{
		picMasterCommand.Write(0x20);
		if(0x28 <= interruptNumber)
			picSlaveCommand.Write(0x20);
	}

	return esp;
}
