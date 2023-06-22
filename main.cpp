#include "mbed.h"
#include "Scheduler/Scheduler.h"
#include "MAX32620FTHR.h"

 // Initialize I/O voltages on MAX32620FTHR board
 MAX32620FTHR fthr(MAX32620FTHR::VIO_3V3);

Serial pc(USBTX,USBRX,115200); // 3rd parameter is baud rate


int main()
{
    pc.printf("MBED VERSION %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    if(Scheduler_Init())
    {
        pc.printf("Initialization failed, Device restarts/n");
        wait_ms(1000);
        NVIC_SystemReset();
        //printf Failure message
        //wait 10s and issue POR
        while(1);
    }

    for (;;) 
    {
        Scheduler_Loop();
    }
}
