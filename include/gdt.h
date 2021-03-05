#ifndef __NUGAOS__GDT_H
#define __NUGAOS__GDT_H

#include <common/types.h>

namespace nugaos
{
    
    class GlobalDescriptorTable
    {
        public:

            class SegmentDescriptor
            {
                private:
                    nugaos::common::uint16_t limit_lo;
                    nugaos::common::uint16_t base_lo;
                    nugaos::common::uint8_t base_hi;
                    nugaos::common::uint8_t type;
                    nugaos::common::uint8_t limit_hi;
                    nugaos::common::uint8_t base_vhi;

                public:
                    SegmentDescriptor(nugaos::common::uint32_t base, nugaos::common::uint32_t limit, nugaos::common::uint8_t type);
                    nugaos::common::uint32_t Base();
                    nugaos::common::uint32_t Limit();
            } __attribute__((packed));

        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:

            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            nugaos::common::uint16_t CodeSegmentSelector();
            nugaos::common::uint16_t DataSegmentSelector();
    };

}
    
#endif