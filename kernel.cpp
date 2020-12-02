
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

void printf(char* str)
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0, y=0;

    for(int i = 0; str[i] != '\0'; ++i)
	{

    	switch(str[i])
    	{
    		case '\n':
    			y++;
    			x = 0;
    			break;

    		default:
    			VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
    			x++;
    			break;
    	}

        

    	if(x >= 80)
    	{
    		y++;
    		x = 0;
    	}
    	if(y >= 25)
    	{
    		for(y = 0; y < 25; y++)
    			for(x=0; x<80; x++)
    				VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
    			
    		x = 0;
    		y = 0;
    	}
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}


typedef void (*constructor)();	//define pointer to constructor function
extern "C" constructor* start_ctors;
extern "C" constructor* end_ctors;
extern "C" void callConstructors()
{
	for(constructor* i = start_ctors; i != end_ctors; i++)
		(*i)();		// invoke constructor calls
}

extern "C" void kernelMain(void* multiboot_structure, uint32_t /*magicnumber*/)
{
    printf("Welcome to NugaOS!\n\n");


    // instantiate table
    printf("Initializing GTD & IDT...");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    // instantiate hardware
    printf("\nInitializing Keyboard...");
    KeyboardDriver keyboard(&interrupts);
    // activate hardware

    // activate interrupt
    interrupts.Activate();
    printf("\nInterrupt Activated.");

    while(1);
}
