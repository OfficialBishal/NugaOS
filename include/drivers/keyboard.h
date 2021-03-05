
#ifndef __NUGAOS__DRIVERS__KEYBOARD_H
#define __NUGAOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace nugaos
{
    namespace drivers
    {
    
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };
        
        class KeyboardDriver : public nugaos::hardwarecommunication::InterruptHandler, public Driver
        {
            nugaos::hardwarecommunication::Port8Bit dataport;
            nugaos::hardwarecommunication::Port8Bit commandport;
            
            KeyboardEventHandler* handler;
        public:
            KeyboardDriver(nugaos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler *handler);
            ~KeyboardDriver();
            virtual nugaos::common::uint32_t HandleInterrupt(nugaos::common::uint32_t esp);
            virtual void Activate();
        };

    }
}
    
#endif