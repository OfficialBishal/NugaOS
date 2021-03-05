
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>


// #define GRAPHICSMODE


using namespace nugaos;
using namespace nugaos::common;
using namespace nugaos::drivers;
using namespace nugaos::hardwarecommunication;
using namespace nugaos::gui;


void processdata(char* data);
uint8_t stringCompare(char* mj1, char* mj2);
void inttostring(uint16_t data);

void sysadmin();
void calc();
void help();

void sysdata(char* data);
void calcdata(char* data);
void helpdata(char* data);


uint8_t mainid=0;
uint8_t calcid=0;

void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        VideoMemory[80*y+x+1] = (VideoMemory[80*y+x+1] & 0xFF00) | ' ';
        VideoMemory[80*y+x+2] = (VideoMemory[80*y+x+2] & 0xFF00) | ' ';


        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            case '\b':
                if(x==0)
                    break;
                x = x - 1;
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        VideoMemory[80*y+x+1] = (VideoMemory[80*y+x+1] & 0xFF00) | '<';
        VideoMemory[80*y+x+2] = (VideoMemory[80*y+x+2] & 0xFF00) | '-';

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
        if(stringCompare(str,"xxbkgxx"))
        {

            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
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
void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}



class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:

    char input[100];
    char data[100];
    
    uint8_t i = 0;
    uint8_t j = 0;


    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);

        if(c == '\b')
        {
            i--;
            input[i] = '\0';
        }
        else if (c == '\n')
        {
            input[i]='\0';
            
            for (j =0; input[j] != '\0'; j++)
                {data[j] = input[j];}
            // printf(input);uint8_t
            
                processdata(data);  
            
            
           // printf(input);
           // printf(data);
            for (j =0; input[j] != '\0'; j++)
             {
                input[j] = '\0';
                data[j] = '\0';
             }
           i=0;
        }
        else if (c != '\n' & c!= '\b')
        {   
            input[i] = foo[0];
            i++;
        }

    }
};


void clear()
{
    uint32_t i =0;
    printf("Clearing the screen...");
    for (i=0; i<1000000000; i++){

    }
   printf("xxbkgxx"); 
}

uint8_t operatorchecker(char* input){
    uint8_t i=0;
    char operatortype[5];
    for(i=0; i<3; i++){
        operatortype[i] = input[i];
    }
    operatortype[i] = '\0';

    if (stringCompare(operatortype, "add") == 1){
        printf("\nAddition Operation Detected.");
        return 1;
    }
    else if (stringCompare(operatortype, "sub") == 1){
        printf("\nSubtraction Operation Detected.");
        return 2;
    }
    else if (stringCompare(operatortype, "mul") == 1){
        printf("\nMultiplication Operation Detected.");
        return 3;
    }
    else if (stringCompare(operatortype, "div") == 1){
        printf("\nDivision Operation Detected.");
        return 4;
    }
}
uint8_t commalocator(char* data){
    printf("\nCalculating the position of comma...");
    uint8_t i;
    for(i=0; data[i]!=','; i++){
    }
    return i;
}

uint8_t len (char* data){
    printf("\nCalculating the length of command...");
    uint8_t i;
    for(i=0; data[i]!='\0'; i++){
    }
    return (i-1);
}

uint16_t chartoint(char* data,uint8_t length){
    uint8_t i;
    uint16_t integer = 0;
    uint16_t temp;
    for(i=0; data[i]!='\0'; i++)
    {
        if (data[i] == '0')
        {
            temp = 0;
        }
        else if (data[i] == '1')
        {
            temp = 1;
        }
        else if (data[i] == '2')
        {
            temp = 2;
        }
        else if (data[i] == '3')
        {
            temp = 3;
        }
        else if (data[i] == '4')
        {
            temp = 4;
        }
        else if (data[i] == '5')
        {
            temp = 5;
        }
        else if (data[i] == '6')
        {
            temp = 6;
        }
        else if (data[i] == '7')
        {
            temp = 7;
        }
        else if (data[i] == '8')
        {
            temp = 8;
        }
        else if (data[i] == '9')
        {
            temp = 9;
        }
        integer = integer * 10 + temp;
    }
    return integer;
}

uint16_t termone(char* data, uint8_t comma)
{
    // printf("\nExtracting first term...");
    uint8_t i;
    char termone[10];
    for(i=4; i<comma; i++)
    {
        termone[i-4] = data[i];
    }
    return chartoint(termone, i-4);
}

uint16_t termtwo(char* data, uint8_t comma, uint8_t length){
    // printf("\nExtracting second term...");
    uint8_t i;
    char termtwo[10];
    for(i=comma+1; i<length; i++)
    {
        termtwo[i-comma-1] = data[i];
    }
    return chartoint(termtwo, i-2);
}



// char* inttostring(uint16_t data)
// {
// uint16_t temp[100],i, ;
// while(data!=0)
// {

// temp= data % 10;

// data= data / 10;
// }

void inttostring(uint16_t data)
{
    char string[100];
    uint8_t i;
    uint16_t temp;
    uint8_t length = 0;
    temp = data;
    while(temp!=0){
        length++;
        temp=temp/10;
    }
    for(i=0; i<length; i++)
    {
        temp = data % 10;
        if(temp == 0)
        {
            string[i] = '0';
        }
        else if(temp == 1)
        {
            string[i] = '1';
        }
        else if(temp == 2)
        {
            string[i] = '2';
        }
        else if(temp == 3)
        {
            string[i] = '3';
        }
        else if(temp == 4)
        {
            string[i] = '4';
        }
        else if(temp == 5)
        {
            string[i] = '5';
        }
        else if(temp == 6)
        {
            string[i] = '6';
        }
        else if(temp == 7)
        {
            string[i] = '7';
        }
        else if(temp == 8)
        {
            string[i] = '8';
        }
        else if(temp == 9)
        {
            string[i] = '9';
        }
        data = data / 10;
    }
    printf(string);
}

void inttostring32(uint32_t data)
{
    char string[100];
    uint8_t i;
    uint32_t temp;
    uint8_t length = 0;
    temp = data;
    while(temp!=0){
        length++;
        temp=temp/10;
    }
    for(i=0; i<length; i++)
    {
        temp = data % 10;
        if(temp == 0)
        {
            string[i] = '0';
        }
        else if(temp == 1)
        {
            string[i] = '1';
        }
        else if(temp == 2)
        {
            string[i] = '2';
        }
        else if(temp == 3)
        {
            string[i] = '3';
        }
        else if(temp == 4)
        {
            string[i] = '4';
        }
        else if(temp == 5)
        {
            string[i] = '5';
        }
        else if(temp == 6)
        {
            string[i] = '6';
        }
        else if(temp == 7)
        {
            string[i] = '7';
        }
        else if(temp == 8)
        {
            string[i] = '8';
        }
        else if(temp == 9)
        {
            string[i] = '9';
        }
        data = data / 10;
    }
    printf(string);
}

void add(char* data){
    uint16_t sum = 0;
    uint8_t comma = commalocator(data);
    uint8_t length = len(data);

    printf("\nComma: ");
    inttostring(comma);
    printf("\nLength: ");
    inttostring(length);
    printf("\nExtracting first term...");
    uint16_t a = termone(data, comma); 
    printf("\nExtracting second term...");
    uint16_t b = termtwo(data, comma, length);
    printf("\nPerforming addition operation...");
    sum = a + b;

    printf("\nAnswer: ");
    inttostring(sum);
}

void sub(char* data){
    uint16_t sub = 0;
    uint8_t comma = commalocator(data);
    uint8_t length = len(data);

    printf("\nComma: ");
    inttostring(comma);
    printf("\nLength: ");
    inttostring(length);
    printf("\nExtracting first term...");
    uint16_t a = termone(data, comma); 
    printf("\nExtracting second term...");
    uint16_t b = termtwo(data, comma, length);

    sub = a - b;

    printf("\nAnswer: ");
    inttostring(sub);
}

void mul(char* data){
    uint16_t mul = 0;
    uint8_t comma = commalocator(data);
    uint8_t length = len(data);

    printf("\nComma: ");
    inttostring(comma);
    printf("\nLength: ");
    inttostring(length);

    printf("\nExtracting first term...");
    uint16_t a = termone(data, comma); 
    printf("\nExtracting second term...");
    uint16_t b = termtwo(data, comma, length);

    mul = a * b;

    printf("\nAnswer: ");
    inttostring(mul);
}

void div(char* data){
    uint16_t div = 0;
    uint8_t comma = commalocator(data);
    uint8_t length = len(data);

    printf("\nComma: ");
    inttostring(comma);
    printf("\nLength: ");
    inttostring(length);

    printf("\nExtracting first term...");
    uint16_t a = termone(data, comma); 
    printf("\nExtracting second term...");
    uint16_t b = termtwo(data, comma, length);

    div = a / b;

    printf("\nAnswer: ");
    inttostring(div);
}

void processdata(char* data)
{
    uint8_t comparatorkind = operatorchecker(data);
    // printf("\n");
    if (stringCompare(data, "hi") == 1)
    {
        printf("Hello User.");
       // printf("\n\n");
    }
    else if (stringCompare(data, "exit") == 1)
    {
        printf("See you soon.");
       // printf("\n\n");
    }
     else if (stringCompare(data, "clear") == 1)
    {
        clear();
    }
    else if (stringCompare(data, "help") == 1)
     {
        clear();
        help();
        printf("\n\n root/");
       // printf("\n\n");
     }
     else if (comparatorkind == 1)
     {
        add(data);
     }
     else if (comparatorkind == 2)
     {
        sub(data);
     }
     else if (comparatorkind == 3)
     {
        mul(data);
     }
     else if (comparatorkind == 4)
     {
        div(data);
     }
    else
    {
        printf("Command Unknown.");
        //printf("\n\n");
    }
    printf("\n\n");

}



// void sysadmin()
// {
//     printf("Commands:\nroot/sysadmin/\n");
//     printf("1. for Hardware Inforation\n");
//     printf("2. for Interrupt Checking");
//     mainid=1;
// }
void calc()
{
    printf("Arithmatic Operations\n1.Add\n2.Subtract\n3.Divide\n4.Multiple");
    mainid=2;
}
void help()
{
        printf("root/help/\n");
        printf("functions available:\n 1. clear: To clear sccreen");
        printf("\n 2. hi: To activate cortona");
        printf("\n 3. exit: To exit cortona");
        printf("\n 4. sysadmin: To enter system configuration as admin");
        printf("\n 5. cacl: To perform arithmetic calculation");
        printf("\n 6. help: To ask for help");
}

// void sysdata(char* data)
// {
//     // printf("\n");
//     if (stringCompare(data, "1") == 1)
//     {
//         printf("root/sysadmin/hardware information.\n");
//         printf("\n\n");
//     }
//    else if(stringCompare(data,"2")==1)
//    {
//     printf("root/sysadmin/interrupt checking\n");

//    }
//     else
//     {
//         printf("Command Unknown.");
//     }
// }

void calcdata(char* data)
{
    // printf("\n");
    if (stringCompare(data, "1") == 1)
    {
        printf("root/calc/add\n");
        calcid=1;
    }
   else if(stringCompare(data,"2")==1)
   {
        printf("root/calc/subtract\n");
        calcid=2;
   }
   else if(stringCompare(data,"3")==1)
   {
        printf("root/calc/divide\n");
        calcid=3;
   }
   else if(stringCompare(data,"4")==1)
   {
        printf("root/calc/multiply\n");
        calcid=4;
   }
    else
    {
        printf("Command Unknown.");
    }
}

uint8_t stringCompare(char* mj1, char* mj2)  // function definition
{
    uint8_t i = 0, flag = 0;
    while(mj1[i] != '\0' && mj2[i] != '\0') // until atleast one string ends
    {
        /*
            Even if they differ by a single character, 
            don't iterate further
        */
        if(mj1[i] != mj2[i]) 
        {
            flag = 1;
            break;
        }
        i++;
    }

    /*
        If all the characters are sequentially same as well as 
        both strings have ended
    */
    if(flag == 0 && mj1[i] == '\0' && mj2[i] == '\0')
        return 1;

    else
        return 0;

}

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);        
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};


// void sysprintf(char* str)
// {
//     asm("int $0x80" : : "a" (4), "b" (str));
// }

// void taskA()
// {
//     while(true)
//         sysprintf("A");
// }

// void taskB()
// {
//     while(true)
//         sysprintf("B");
// }




typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure /* getting from grub */, uint32_t /*multiboot_magic*/)
{
    printf("\n                    ---------- Welcome to NugaOS ----------\n\n\n");
    printf("\nMinor Project By:");
    printf("\nBishal Shrestha (ACE074BCT016)");
    printf("\nGaurab Silwal (ACE074BCT018)");
    printf("\nKritika Simkhada (ACE074BCT024)\n\n");

  

    // Initialize GDT
    GlobalDescriptorTable gdt;
    

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);    // tells size of RAM in KB
    size_t heap = 10*1024*1024; //10MB

    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);   //10KB safe guard behind heap
    
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);
    
    void* allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n\n");
    
    TaskManager taskManager;
    /*
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */

    // Initialize Interrupt (IDT)
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    
   // printf("Initializing Hardware, Stage 1\n");
    
    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif
    
    DriverManager drvManager;
    
        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);
        
    
        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);
        
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        #ifdef GRAPHICSMODE
            VideoGraphicsArray vga;     // vga object of VGA
        #endif
        
   // printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();

    //printf("Initializing Hardware, Stage 3\n");

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif

    interrupts.Activate();

    // printf("\n\n\n\n");
    // printf("END1");
    // GlobalDescriptorTable* gdtdisplay = &gdt;
    // uint32_t CodeSegmentForDisplay = gdtdisplay->CodeSegmentSelector();
    // uint8_t letssee = &CodeSegmentForDisplay->type;

    // inttostring32(CodeSegmentForDisplay);

    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }

    // printf("END2");

}
