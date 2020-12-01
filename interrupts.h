
#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "gdt.h"
#include "port.h"

	class InterruptManager;

	class InterruptHandler
	{
	protected:
	    uint8_t interruptNumber;
	    InterruptManager* interruptManager;
	    InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
	    ~InterruptHandler();
	public:
	    virtual uint32_t HandleInterrupt(uint32_t esp);
	};


	class InterruptManager
	{

	friend class InterruptHandler;

	protected:

		static InterruptManager* ActiveInterruptManager;	//pointer to 1 active interrupt
		InterruptHandler* handlers[256];

		// Entries of IDT is called GateDescriptor
		struct GateDescriptor
		{
			uint16_t handlerAddressLowBits;
			uint16_t gdt_codeSegmentSelector;
			uint8_t reserved;
			uint8_t access;
			uint16_t handlerAddressHighBits;
		} __attribute__((packed));

		// array with 256 entries
		static GateDescriptor interruptDescriptorTable[256];

		// tell processor to use IDT entries
		struct InterruptDescriptorTablePointer
		{
			uint16_t size;
			uint32_t base;
		} __attribute__((packed));


		// set entries in IDT
		static void SetInterruptDescriptorTableEntry(
			uint8_t interruptNumber,
			uint16_t codeSegmentSelectorOffset,
			void (*handler)(),
			uint8_t DescriptorPrivilegeLevel,
			uint8_t DescriptorType
		);

		// tell PIC(Programmable Interrupt Controller) to give interrupt
		// 2 PIC =  Master PIC and Slave PIC
		Port8BitSlow picMasterCommand;
        Port8BitSlow picMasterData;
        Port8BitSlow picSlaveCommand;
        Port8BitSlow picSlaveData;



	public:

		InterruptManager(GlobalDescriptorTable* gdt);
		~InterruptManager();

		// start and stop the interrupt after being loaded into processor
		void Activate();
		void Deactivate();

		// .extern _ZN16InterruptManager15handleInterruptEhj
		static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
		uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

		static void IgnoreInterruptRequest();

		static void HandleInterruptRequest0x00();	//timer interrupt
		static void HandleInterruptRequest0x01();	//keyboard interrupt

	};



#endif