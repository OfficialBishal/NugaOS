
#include <drivers/keyboard.h>

using namespace nugaos::common;
using namespace nugaos::drivers;
using namespace nugaos::hardwarecommunication;


KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

void KeyboardEventHandler::OnKeyUp(char)
{
}





KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler)
:   InterruptHandler(manager, 0x21),
    dataport(0x60),
    commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);
void printfHex(uint8_t);

void KeyboardDriver::Activate()
{
    // Configuring the keyboard
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10; // set rightmost bit to 1 & clear the 5th state
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    
    dataport.Write(0xf4);  //activate keyboard
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    static bool Shift = false;
    
    if(handler == 0)
        return esp;
    
    if(key < 0x80)  // when pressing 0x1E & releasing 0x9E (different first bit)
    {
        switch(key)
        {
            case 0x02: handler->OnKeyDown('1'); break;
            case 0x03: handler->OnKeyDown('2'); break;
            case 0x04: handler->OnKeyDown('3'); break;
            case 0x05: handler->OnKeyDown('4'); break;
            case 0x06: handler->OnKeyDown('5'); break;
            case 0x07: handler->OnKeyDown('6'); break;
            case 0x08: handler->OnKeyDown('7'); break;
            case 0x09: handler->OnKeyDown('8'); break;
            case 0x0A: handler->OnKeyDown('9'); break;
            case 0x0B: handler->OnKeyDown('0'); break;

            case 0x10: handler->OnKeyDown('q'); break;
            case 0x11: handler->OnKeyDown('w'); break;
            case 0x12: handler->OnKeyDown('e'); break;
            case 0x13: handler->OnKeyDown('r'); break;
            case 0x14: handler->OnKeyDown('t'); break;
            case 0x15: handler->OnKeyDown('y'); break;
            case 0x16: handler->OnKeyDown('u'); break;
            case 0x17: handler->OnKeyDown('i'); break;
            case 0x18: handler->OnKeyDown('o'); break;
            case 0x19: handler->OnKeyDown('p'); break;

            case 0x1E: handler->OnKeyDown('a'); break;
            case 0x1F: handler->OnKeyDown('s'); break;
            case 0x20: handler->OnKeyDown('d'); break;
            case 0x21: handler->OnKeyDown('f'); break;
            case 0x22: handler->OnKeyDown('g'); break;
            case 0x23: handler->OnKeyDown('h'); break;
            case 0x24: handler->OnKeyDown('j'); break;
            case 0x25: handler->OnKeyDown('k'); break;
            case 0x26: handler->OnKeyDown('l'); break;

            case 0x2C: handler->OnKeyDown('z'); break;
            case 0x2D: handler->OnKeyDown('x'); break;
            case 0x2E: handler->OnKeyDown('c'); break;
            case 0x2F: handler->OnKeyDown('v'); break;
            case 0x30: handler->OnKeyDown('b'); break;
            case 0x31: handler->OnKeyDown('n'); break;
            case 0x32: handler->OnKeyDown('m'); break;
            case 0x33: handler->OnKeyDown(','); break;
            case 0x34: handler->OnKeyDown('.'); break;
            case 0x35: handler->OnKeyDown('/'); break;

            case 0x27: handler->OnKeyDown(';'); break;
            case 0x1A: handler->OnKeyDown('['); break;
            case 0x1B: handler->OnKeyDown(']'); break;
            case 0x0C: handler->OnKeyDown('-'); break;
            case 0x0D: handler->OnKeyDown('='); break;
            case 0x29: handler->OnKeyDown('`'); break;


            case 0x1C: handler->OnKeyDown('\n'); break;
            case 0x39: handler->OnKeyDown(' '); break;

            case 0x0E: handler->OnKeyDown('\b'); break;

            // case 0x3A: 
            //     uint8_t checker = 0;
            //     if checker == 0
            //         Shift = true;
            //         checker = 1; 
            //     else 
            //         Shift = false;
            //         checker = 0;
            //     break;

            // case 0x2A: case 0x36: Shift = true; break;
            // case 0xAA: case 0xB6: Shift = false; break;

            default:
            {
                printf("KEYBOARD 0x");
                printfHex(key);
                printf("\n\n");
                break;
            }
        }
    }
    return esp;
}
