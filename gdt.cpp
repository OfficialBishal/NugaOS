#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable()
	: 	nullSegmentSelector(0,0,0),
		unusedSegmentSelector(0,0,0),
		codeSegmentSelector(0, 64*1024*1024, 0x9A),
		dataSegmentSelector(0,64*1024*1024, 0x92)
{

//tell processor to use GDT
//Processor expects 8bytes in a row
	uint32_t i[2];
	i[0] = (uint32_t)this;
	i[1] = sizeof(GlobalDescriptorTable) << 16;	//shift to left cause it needs to be in high bytes.

	asm volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));	//tell asm to load GDT


}


GlobalDescriptorTable::~GlobalDescriptorTable()
{
	//unload GDT
}


//we want offset of data segment selector
uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
	return(uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}
//we want offset of code segment selector
uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
	return(uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}

//hard part: to set entries according to the parameters we get here
GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags)
{

	uint8_t* target = (uint8_t*)this;

	if(limit <= 65536)	//16bit=65534
	{
		target[6] = 0x40;	//tell processor this is 16bit entry
	}
	else
	{
		// 32-bit address space
        // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
        // This is done by discarding the 12 least significant bits, but this
        // is only legal, if they are all ==1, so they are implicitly still there

        // so if the last bits aren't all 1, we have to set them to 1, but this
        // would increase the limit (cannot do that, because we might go beyond
        // the physical limit or get overlap with other segments) so we have to
        // compensate this by decreasing a higher bit (and might have up to
        // 4095 wasted bytes behind the used memory)
		
		if((limit & 0xFFF) != 0xFFF)	//if last 12bit are not all 1
			limit = (limit >>12)-1;
		else
			limit = (limit >>12);

		target[6] = 0xC0;
	}

	target[0] = limit & 0xFF;			//lsb of limit
	target[1] = limit >> 8 & 0xFF;
	target[6] |= (limit >> 16) & 0xF;	// |= to set only lower bits 

	target[2] = (base) & 0xFF;
	target[3] = (base >> 8) & 0xFF;
	target[4] = (base >> 16) & 0xFF;
	target[7] = (base >> 24) & 0xFF;

	target[5] = flags;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
	//isn't that hard cause all 4bytes are preserved
	uint8_t* target = (uint8_t*)this;
	uint32_t result = target[7];
	result = (result << 8) + target[4];
	result = (result << 8) + target[3];
	result = (result << 8) + target[2];
	return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
	
	uint8_t* target = (uint8_t*)this;
	uint32_t result = target[6] & 0xF;
	result = (result << 8) + target[1];
	result = (result << 8) + target[0];

	if((target[6] & 0xC0) == 0xC0)
		result = (result << 12) | 0xFFF;

	return result;
}