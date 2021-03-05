 
#ifndef __NUGAOS__SYSCALLS_H
#define __NUGAOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace nugaos
{
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {
        
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, nugaos::common::uint8_t InterruptNumber);
        ~SyscallHandler();
        
        virtual nugaos::common::uint32_t HandleInterrupt(nugaos::common::uint32_t esp);

    };
    
    
}


#endif