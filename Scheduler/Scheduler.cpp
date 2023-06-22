#include "mbed.h"
#include "Scheduler.h"
#include "MAX17841_Interface/MAX17841_Interface.h"

extern Serial pc;
Timer scheduler;

DigitalOut dbg2(P1_5);
DigitalOut SHDNL(P0_2);

// MCU board status LEDs
DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

#define RGBLED_WHITE    rLED = LED_ON;    bLED = LED_ON;    gLED = LED_ON
#define RGBLED_RED      rLED = LED_ON;    bLED = LED_OFF;    gLED = LED_OFF
#define RGBLED_GREEN    rLED = LED_OFF;   bLED = LED_OFF;    gLED = LED_ON
#define RGBLED_BLUE     rLED = LED_OFF;   bLED = LED_ON;    gLED = LED_OFF
#define RGBLED_PURPLE   rLED = LED_ON;    bLED = LED_ON;    gLED = LED_OFF

//Wakeup Variables
char u5DaisyChainSize = 0;  //Size of the daisy Chain (will be written at HELLOALL)
unsigned long waketime=0;   //Time the chain takes to wake up (in us)

short ConfigData[50];
short readAllReadback[20];


char Scheduler_Init(void)
{  
    int i, j;

    //Initialization: WHITE
    RGBLED_WHITE;

    scheduler.start();

    SHDNL = true;

    if(MAX17841_Init(&waketime, &u5DaisyChainSize) != 0x00)
    {
        pc.printf("Daisy Chain Initialization failed\r\n");
        return 1;
    }
    else {
        pc.printf("Chain woke up in %u Microseconds\r\n", waketime);
        pc.printf("MAX17851 Initialized. Chain Size = %d\r\n", u5DaisyChainSize);
        MAX17852_writeAll('M', DEVCFG1, 0x0108, 100);     //Single UART, UART HOST, DC Enable
        MAX17852_writeAll('M', STATUS1, 0x0000, 100);     //Clear ALRTRESET
        MAX17852_writeAll('M', STATUS2, 0x0000, 100);     //Clear Status
        MAX17852_writeAll('M', STATUS3, 0x0000, 100);     //Clear Status
        MAX17852_writeAll('M', PACKCFG, 0x0FFF, 100);     //Disable Flexpack
        MAX17852_writeAll('M', AUXGPIOCFG, 0x0000, 100);  //Enable ADIO
        MAX17852_writeAll('M', MEASUREEN1, 0x3FFF, 100);  //Enable All Cells but no block
        MAX17852_writeAll('M', MEASUREEN2, 0x003F, 100);  //enable AUX


        //Wait until SCAN finishes (time to read out configuration)
        for(j=0;j<u5DaisyChainSize;j++)
        {
            MAX17852_readBlock('M', 0, 0x00, 19);   //Read Status
            wait_us(2000);
            MAX17852_readBlock_RB('M', &ConfigData[0]);   //Transfer data to buffer of first device
            pc.printf("BMSIC Config %d: ", j);
            for(i=0;i<19;i++)
            {
                pc.printf("%04x ", (unsigned short)ConfigData[i]);
            }
            pc.printf("\r\n");
        }
        
        //Initialization Done OK: GREEN
        RGBLED_GREEN;

        pc.printf("Loop Start\r\n");
        return 0;
    }
}

void Scheduler_Loop(void)
{
    int i,j;

    MAX17852_writeAll('M', DIAGCFG, 0x00AB, 100);    //DIAG1 = ADC Zero Scale, DIAG2 = ADC Full Scale
    MAX17852_writeAll('M', SCANCTRL, 0x0109, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8, HVMUX
    wait_ms(20);
    for(j=0;j<14;j++)
    {
        MAX17852_readAll('M', CELL1REG+j);
        wait_us(200);
        MAX17852_readAll_RB('M',&readAllReadback[0]);

        pc.printf("C%02d", j+1);   //CELL 1...14

        for(i=0;i<u5DaisyChainSize;i++)
        {
            pc.printf(" D%d %04x", i,(unsigned short)readAllReadback[i]);    //DEV 0...x
        }
        pc.printf("\r\n");
    }

    wait_ms(1000);
}
