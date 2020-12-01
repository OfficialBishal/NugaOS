
#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
	while(commandport.Read() & 0x1)
		dataport.Read();	//wait for key to be unpressed and clear after released
	commandport.Write(0xAE);	// activate interrupt : tell PIC to start sendings
	commandport.Write(0x20);	// get current state : read controller command byte
	uint8_t status = (dataport.Read() | 1) & ~0x10;	// set rightmost bit to 1 & clear the 5th state
	commandport.Write(0x60);	// set state : set controller command byte
	dataport.Write(status);

	dataport.Write(0xF4);	// activate the keyboard 
}	

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);
void printfHex(uint8_t);


uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
	uint8_t key = dataport.Read();

	if(key <0x80)// when pressing 0x1E & releasing 0x9E (different first bit)
	{
		switch(key)
		{
			case 0xFA: break;
			case 0x45: case 0xC5: break;

			default:
			{								
				printf("KEYBOARD 0x");
				printfHex(key);
				break;
			}
		}

		return esp;
	}
}
