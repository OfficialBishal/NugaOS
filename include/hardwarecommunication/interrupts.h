
#ifndef __NUGAOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H
#define __NUGAOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H

#include <gdt.h>
#include <multitasking.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>


namespace nugaos
{
    namespace hardwarecommunication
    {

        class InterruptManager;

        class InterruptHandler
        {
        protected:
            nugaos::common::uint8_t InterruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(InterruptManager* interruptManager, nugaos::common::uint8_t InterruptNumber);
            ~InterruptHandler();
        public:
            virtual nugaos::common::uint32_t HandleInterrupt(nugaos::common::uint32_t esp);
        };


        class InterruptManager
        {
            friend class InterruptHandler;
            protected:

                static InterruptManager* ActiveInterruptManager;    //pointer to 1 active interrupt
                InterruptHandler* handlers[256];
                TaskManager *taskManager;

                // Entries of IDT is called GateDescriptor
                struct GateDescriptor
                {
                    nugaos::common::uint16_t handlerAddressLowBits;
                    nugaos::common::uint16_t gdt_codeSegmentSelector;
                    nugaos::common::uint8_t reserved;
                    nugaos::common::uint8_t access;
                    nugaos::common::uint16_t handlerAddressHighBits;
                } __attribute__((packed));

                // array with 256 entries that point to 256 interrupts.
                static GateDescriptor interruptDescriptorTable[256];

                // tell processor to use IDT entries
                struct InterruptDescriptorTablePointer
                {
                    nugaos::common::uint16_t size;
                    nugaos::common::uint32_t base;
                } __attribute__((packed));

                nugaos::common::uint16_t hardwareInterruptOffset;

                // set entries in IDT
                static void SetInterruptDescriptorTableEntry(
                    nugaos::common::uint8_t interrupt,
                    nugaos::common::uint16_t codeSegmentSelectorOffset, 
                    void (*handler)(),
                    nugaos::common::uint8_t DescriptorPrivilegeLevel, 
                    nugaos::common::uint8_t DescriptorType);


                static void InterruptIgnore();

                static void HandleInterruptRequest0x00();   // timer interrupt
                static void HandleInterruptRequest0x01();   //keyboard interrupt
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();

                static void HandleInterruptRequest0x80();

                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();
                
                static nugaos::common::uint32_t HandleInterrupt(nugaos::common::uint8_t interrupt, nugaos::common::uint32_t esp);
                nugaos::common::uint32_t DoHandleInterrupt(nugaos::common::uint8_t interrupt, nugaos::common::uint32_t esp);

                // tell PIC(Programmable Interrupt Controller) to give interrupt
                // 2 PIC =  Master PIC and Slave PIC
                Port8BitSlow programmableInterruptControllerMasterCommandPort;
                Port8BitSlow programmableInterruptControllerMasterDataPort;
                Port8BitSlow programmableInterruptControllerSlaveCommandPort;
                Port8BitSlow programmableInterruptControllerSlaveDataPort;

            public:
                InterruptManager(nugaos::common::uint16_t hardwareInterruptOffset, nugaos::GlobalDescriptorTable* globalDescriptorTable, nugaos::TaskManager* taskManager);
                ~InterruptManager();
                nugaos::common::uint16_t HardwareInterruptOffset();

                // start and stop the interrupt after being loaded into processor
                void Activate();
                void Deactivate();

        };
        
    }
}

#endif