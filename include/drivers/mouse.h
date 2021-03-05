
#ifndef __NUGAOS__DRIVERS__MOUSE_H
#define __NUGAOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace nugaos
{
    namespace drivers
    {
    
        class MouseEventHandler
        {
        public:
            MouseEventHandler();

            virtual void OnActivate();
            virtual void OnMouseDown(nugaos::common::uint8_t button);
            virtual void OnMouseUp(nugaos::common::uint8_t button);
            virtual void OnMouseMove(int x, int y);
        };
        
        
        class MouseDriver : public nugaos::hardwarecommunication::InterruptHandler, public Driver
        {
            nugaos::hardwarecommunication::Port8Bit dataport;
            nugaos::hardwarecommunication::Port8Bit commandport;
            nugaos::common::uint8_t buffer[3];
            nugaos::common::uint8_t offset;
            nugaos::common::uint8_t buttons;

            MouseEventHandler* handler;
        public:
            MouseDriver(nugaos::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual nugaos::common::uint32_t HandleInterrupt(nugaos::common::uint32_t esp);
            virtual void Activate();
        };

    }
}
    
#endif
