 
#ifndef __NUGAOS__HARDWARECOMMUNICATION__PCI_H
#define __NUGAOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>

#include <memorymanagement.h>

namespace nugaos
{
    namespace hardwarecommunication
    {

        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };
        
        
        
        class BaseAddressRegister
        {
        public:
            bool prefetchable;
            nugaos::common::uint8_t* address;
            nugaos::common::uint32_t size;
            BaseAddressRegisterType type;
        };
        
        
        
        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            nugaos::common::uint32_t portBase;
            nugaos::common::uint32_t interrupt;
            
            nugaos::common::uint16_t bus;
            nugaos::common::uint16_t device;
            nugaos::common::uint16_t function;

            nugaos::common::uint16_t vendor_id;
            nugaos::common::uint16_t device_id;
            
            nugaos::common::uint8_t class_id;
            nugaos::common::uint8_t subclass_id;
            nugaos::common::uint8_t interface_id;

            nugaos::common::uint8_t revision;
            
            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
            
        };


        class PeripheralComponentInterconnectController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;
            
        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();
            
            nugaos::common::uint32_t Read(nugaos::common::uint16_t bus, nugaos::common::uint16_t device, nugaos::common::uint16_t function, nugaos::common::uint32_t registeroffset);
            void Write(nugaos::common::uint16_t bus, nugaos::common::uint16_t device, nugaos::common::uint16_t function, nugaos::common::uint32_t registeroffset, nugaos::common::uint32_t value);
            bool DeviceHasFunctions(nugaos::common::uint16_t bus, nugaos::common::uint16_t device);
            
            void SelectDrivers(nugaos::drivers::DriverManager* driverManager, nugaos::hardwarecommunication::InterruptManager* interrupts);
            nugaos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, nugaos::hardwarecommunication::InterruptManager* interrupts);
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(nugaos::common::uint16_t bus, nugaos::common::uint16_t device, nugaos::common::uint16_t function);
            BaseAddressRegister GetBaseAddressRegister(nugaos::common::uint16_t bus, nugaos::common::uint16_t device, nugaos::common::uint16_t function, nugaos::common::uint16_t bar);
        };

    }
}
    
#endif