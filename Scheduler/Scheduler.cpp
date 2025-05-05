#include "mbed.h"
#include "Scheduler.h"
#include "MAX17841_Interface/MAX17841_Interface.h"

#define MeasureCellEn 0x0F0F

char SM24_Execution(void);
char BALSW_OPEN_Diagnostic(void);
char SCAN_Execution(unsigned short set_MEASUREEN, unsigned short set_DIAGEN, unsigned short set_SCANCTRL);
char SCAN_GetMinMax(unsigned short* min, unsigned short* max);
char SM2_CnPinOpenDiag(void);

extern Serial pc;
Timer timebase;
int oldtick_us;

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
char u5DaisyChainSize = 0;      //Size of the daisy Chain (will be written at HELLOALL)
unsigned long waketime=0;       //Time the chain takes to wake up (in us)

//  Incrementing pointer goes left to right and then continuing next
//  ReadALL fills all devices, so we have to select the ROW for the parameter and the column increments automatically
//  D0[3][3] pointer to D0 -> pointer increment (left to right)
//  Param1: D0[0][0] D1[0][1] D2[0][2] D3[0][3]
//  Param2: D0[1][0] D1[1][1] D2[1][2] D3[1][3]
//  Param3: D0[2][0] D1[2][1] D2[2][2] D3[2][3]

//  unsigned short ConfigData[153][32];          //[Number of Parameters][Number of devices]
unsigned short readAllReadback[153][32];     //[Number of Parameters][Number of devices]

int loopcounter, testmode;
unsigned short MinCell[32], MaxCell[32];
unsigned short readAllHVMUX[12][8]; //12 cells max. 8 devices
unsigned short readAllALTMUX[12][8]; //12 cells max. 8 devices
unsigned short readAllMinMaxCell[8];

unsigned short readStatus[8];
unsigned short readAllCellTemp[12][8];


char Scheduler_Init(void)
{  
    char reg, dev;

    //Initialization: WHITE
    RGBLED_WHITE;
    //Needs to be here for timeouts to work
    timebase.start();

    SHDNL = true;

    if(MAX17841_Init(&u5DaisyChainSize) != 0x00)
    {
        pc.printf("Daisy Chain Initialization failed\r\n");
        return 1;
    }
    else 
    {
        pc.printf("MAX17851 Initialized. Chain Size = %d\r\n", u5DaisyChainSize);

        if(u5DaisyChainSize == 0)
        {
            pc.printf("Not Initialized properly\n\r");
            SHDNL = false;
            wait_ms(100);
            Scheduler_Init();
        }

        MAX17823_writeAll('M', STATUS, 0x0000, 200);        //Clear ALRTRESET
        MAX17823_writeAll('M', FMEA1, 0x0000, 200);         //Clear ALRTRESET
        MAX17823_writeAll('M', MEASUREEN, 0x0F0F, 200);     //Enable CELL1-4, 9-12
        MAX17823_writeAll('M', ADCTEST1A, 0x0100, 200);  //Set ADCTEST registers
        MAX17823_writeAll('M', ADCTEST1B, 0x0101, 200);  //Set ADCTEST registers
        MAX17823_writeAll('M', ADCTEST2A, 0x0102, 200);  //Set ADCTEST registers
        MAX17823_writeAll('M', ADCTEST2B, 0x0103, 200);  //Set ADCTEST registers

        MAX17823_writeAll('M', ALRTOVEN, 0x0F0F, 200);  //Enable bottom 4 and Top 4 cells for ALERT
        MAX17823_writeAll('M', ALRTUVEN, 0x0F0F, 200);  //Enable bottom 4 and Top 4 cells for ALERT
        MAX17823_writeAll('M', OVTHSET, 0xD000, 200);  //Enable bottom 4 and Top 4 cells for ALERT
        MAX17823_writeAll('M', OVTHCLR, 0xD000, 200);  //Enable bottom 4 and Top 4 cells for ALERT
        MAX17823_writeAll('M', UVTHSET, 0x9000, 200);  //Enable bottom 4 and Top 4 cells for ALERT
        MAX17823_writeAll('M', UVTHCLR, 0x9000, 200);  //Enable bottom 4 and Top 4 cells for ALERT




        //CONFIG Readout-------------------------------------------------
        // pc.printf("Configuration:\r\n");
        // pc.printf("--------------\r\n");
        // for(row=0;row<0x5A;row++)
        // {
        //     MAX17823_readAll('M', (short)row);
        //     wait_us(600);
        //     MAX17823_readAll_RB('M',&ConfigData[row][0]);
        // }
        // pc.printf("REG DEVn..DEV1 DEV0\r\n");

        // for(row=0;row<153;row++)
        // {
        //     pc.printf("%02x",row);
        //     for(col=0;col<u5DaisyChainSize;col++)
        //     {
        //         pc.printf(" %04x", (unsigned short)ConfigData[row][col]);
        //     }
        //     pc.printf("\r\n");
        // }

        pc.printf("First Scan:\r\n");
        pc.printf("-------------\r\n");
        SCAN_Execution(MeasureCellEn, 0x0007, 0x0031);
        wait_us(6000);
        for(dev=0;dev<u5DaisyChainSize;dev++)
        {
            pc.printf("%d: CELL ", dev);
            for(reg=0;reg<12;reg++)
            {
                MAX17823_readAll('M', CELL1 + reg);     //Reading all cells
                wait_us(600);
                MAX17823_readAll_RB('M', &readAllHVMUX[reg][dev]);
                pc.printf("%04x ", readAllHVMUX[reg][dev]);
            }
            MAX17823_readAll('M', MINMAXCELL);
            wait_us(600);
            MAX17823_readAll_RB('M', readAllMinMaxCell); //Reading MINMAX
            pc.printf("\n\r%d: MINMAX %04x\n\r", dev, readAllMinMaxCell[dev]);
        }
        // SM24_Execution();
        // BALSW_OPEN_Diagnostic();

        pc.printf("Scheduler Start\r\n");
        timebase.reset();
        oldtick_us = timebase.read_us();

        // Initialization Done OK: GREEN
        RGBLED_GREEN;

        loopcounter = 0;
        testmode = 0;
        return 0;
    }
}


void Scheduler_Loop(void)
{
    char reg, dev;

    if(timebase.read_us() > (oldtick_us + 1000))    //1000us have passed
    {
        oldtick_us = timebase.read_us();
        dbg2 = (dbg2 == false) ? true : false;  //Toggle dbg2 (Pin 1.5)

        switch (++loopcounter) 
        {
            case 10:
                pc.printf("%d: HVSCAN\n\r",loopcounter);
                SCAN_Execution(MeasureCellEn, 0x0007, 0x0011);
                break;
            case 20:
                pc.printf("%d: ",loopcounter);
                for(dev=0;dev<u5DaisyChainSize;dev++)
                {
                    MAX17823_readAll('M', STATUS);     //Reading all cells
                    wait_us(600);
                    MAX17823_readAll_RB('M', &readStatus[dev]);
                    pc.printf("%d STATUS %04x\n\r", dev, readStatus[dev]);
                    pc.printf("%d: %d CELL ", loopcounter, dev);
                    for(reg=0;reg<12;reg++)
                    {
                        MAX17823_readAll('M', CELL1 + reg);     //Reading all cells
                        wait_us(600);
                        MAX17823_readAll_RB('M', &readAllHVMUX[reg][dev]);
                        pc.printf("%04x ", readAllHVMUX[reg][dev]);
                    }
                }
                pc.printf("\n\r");
                break;
            case 30:
                SCAN_Execution(MeasureCellEn, 0x000F, 0x0011);
                pc.printf("%d: ALTSCAN\n\r",loopcounter);
                break;
            case 40:
                pc.printf("%d: ",loopcounter);
                for(dev=0;dev<u5DaisyChainSize;dev++)
                {
                    MAX17823_readAll('M', STATUS);     //Reading all cells
                    wait_us(600);
                    MAX17823_readAll_RB('M', &readStatus[dev]);
                    pc.printf("%d: STATUS %04x\n\r", dev, readStatus[dev]);
                    pc.printf("%d: %d CELL ", loopcounter, dev);
                    for(reg=0;reg<12;reg++)
                    {
                        MAX17823_readAll('M', CELL1 + reg);     //Reading all cells
                        wait_us(600);
                        MAX17823_readAll_RB('M', &readAllALTMUX[reg][dev]);
                        pc.printf("%04x ", readAllALTMUX[reg][dev]);
                    }
                }
                pc.printf("\n\r");
                break;
            case 50:
                SM2_CnPinOpenDiag();
                pc.printf("%d: SM2\n\r",loopcounter);
                break;
            case 60:
                pc.printf("%d: ",loopcounter);
                for(dev=0;dev<u5DaisyChainSize;dev++)
                {
                    MAX17823_readAll('M', STATUS);     //Reading all cells
                    wait_us(600);
                    MAX17823_readAll_RB('M', &readStatus[dev]);
                    pc.printf("%d STATUS %04x\n\r", dev, readStatus[dev]);
                    pc.printf("%d: %d CELL ", loopcounter, dev);
                    for(reg=0;reg<12;reg++)
                    {
                        MAX17823_readAll('M', CELL1 + reg);     //Reading all cells
                        wait_us(600);
                        MAX17823_readAll_RB('M', &readAllCellTemp[reg][dev]);
                        pc.printf("%04x ", readAllCellTemp[reg][dev]);
                    }
                }
                pc.printf("\n\r");
                break;
            case 70:
                pc.printf("%d\n\r",loopcounter);
                break;
            case 80:
                pc.printf("%d\n\r",loopcounter);
                break;
            case 90:
                pc.printf("%d\n\r",loopcounter);
                break;
            case 100:
                pc.printf("%d\n\r",loopcounter);
                wait_ms(1000);
                loopcounter = 0;
                break;
            default:
                break;
        }
    }
}


char SCAN_Execution(unsigned short set_MEASUREEN, unsigned short set_DIAGEN, unsigned short set_SCANCTRL)
{
    MAX17823_writeAll('M', DIAGCFG, set_DIAGEN, 200); // DIAG1 = Die Temp, DIAG2 = VAADIAG
    MAX17823_writeAll_RB('M');
    MAX17823_writeAll('M', MEASUREEN, set_MEASUREEN, 200); // DIAG1 = Die Temp, DIAG2 = VAADIAG
    MAX17823_writeAll_RB('M');
    MAX17823_writeAll('M', SCANCTRL, set_SCANCTRL, 200); // SCAN: OS8
    MAX17823_writeAll_RB('M');
    return 0;
}


char SM2_CnPinOpenDiag(void)
{
    MAX17823_writeAll('M', DIAGCFG, 0xF010, 200); //Current Source 100uA, MUXDIAGEN
    MAX17823_writeAll_RB('M');
    MAX17823_writeAll('M', SCANCTRL, 0x0011, 200); // SCAN: OS4
    MAX17823_writeAll_RB('M');
}


char SM24_Execution(void)
{
    int row, col;

    pc.printf("SM24 Execution\n\r");
    pc.printf("--------------\n\r");
    MAX17823_writeAll('M', DIAGCFG, 0x0006, 100);    //DIAG = Die Temp
    MAX17823_writeAll('M', DEVCFG1, 0x0022, 200);    //ADCTSTEN = 1
    MAX17823_writeAll('M', SCANCTRL, 0x0021, 200);   //SCAN: OS8
    wait_us(3000);
    //Readback Status
    for(row=0;row<=0x0B;row++)
    {
        MAX17823_readAll('M', (short)row);
        wait_us(600);
        MAX17823_readAll_RB('M',&readAllReadback[row][0]);  //STATUS = 0x00 - 0x0B
    }
    //Readback SCANCTRL
    MAX17823_readAll('M', SCANCTRL);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x0C][0]);     //SCANCTRL = 0x0C
    //Readback CELL1-12
    for(row=0;row<16;row++)
    {
        MAX17823_readAll('M', CELL1+row);
        wait_us(600);
        MAX17823_readAll_RB('M',&readAllReadback[row+0x0D][0]);    //CELL1-12/BLOCK/AIN1-2/TOTAL = 0x0D - 0x1C
    }
    //Readback DIAG
    MAX17823_readAll('M', DIAG);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1D][0]);     //DIAG = 0x1D
    //Going through all devices in the Daisy Chain and find the faulty SCANCTRL - if so, report it and re-init
    for(col=0;col<u5DaisyChainSize;col++)
    {
        if(((readAllReadback[0x0C][col] & 0xA000) != 0xA000) || ((readAllReadback[0x0C][col]==0x0000)) ||
           (readAllReadback[0x2][col] != 0x0000) || (readAllReadback[0x03][col] != 0x0000))
        {
            pc.printf("0x13[%02d] = %04x\n\r", col, readAllReadback[0x0C][col]);
            SHDNL = false;
            wait_ms(100);
            Scheduler_Init();
        }
    }
    //Readout Results
    for(row=0;row<0x1E;row++)
    {
        if(row <= 0x0B)
        {
            pc.printf("R:%02x", (unsigned short)(row));
        }
        else if (row == 0x0C)
        {
            pc.printf("R:%02x", (unsigned short)(row + SCANCTRL - 0x0C));
        }
        else if ((row >= 0x0D) && (row <= 0x1C))
        {
            pc.printf("R:%02x", (unsigned short)(row + CELL1 - 0x0D));
        }
        else if (row == 0x1D)
        {
            pc.printf("R:%02x", (unsigned short)(row + DIAG - 0x1D));
        }
        else{}
        for(col=0;col<u5DaisyChainSize;col++)
        {
            pc.printf(" %04x", (unsigned short)readAllReadback[row][col]);
        }
        pc.printf("\n\r");
    }   
    MAX17823_writeAll('M', DEVCFG1, 0x0002, 200);    //ADCTSTEN = 0
    return 0;
}

void Readout_Scandone_Fail_Regs(int row, int col)
{
    MAX17823_readAll('M', 0x02);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1F][0]);
    pc.printf("\n\rR:02");
    for(col=0;col<u5DaisyChainSize;col++)
    {
        pc.printf(" %04x", (unsigned short)readAllReadback[0x1F][col]);
    }
    MAX17823_readAll('M', 0x03);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1F][0]);
    pc.printf("\n\rR:03");
    for(col=0;col<u5DaisyChainSize;col++)
    {
        pc.printf(" %04x", (unsigned short)readAllReadback[0x1F][col]);
    }
    MAX17823_readAll('M', 0x0A);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1F][0]);
    pc.printf("\n\rR:0A");
    for(col=0;col<u5DaisyChainSize;col++)
    {
        pc.printf(" %04x", (unsigned short)readAllReadback[0x1F][col]);
    }
    MAX17823_readAll('M', 0x10);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1F][0]);
    pc.printf("\n\rR:10");
    for(col=0;col<u5DaisyChainSize;col++)
    {
        pc.printf(" %04x", (unsigned short)readAllReadback[0x1F][col]);
    }
    MAX17823_readAll('M', 0x13);
    wait_us(600);
    MAX17823_readAll_RB('M',&readAllReadback[0x1F][0]);
    pc.printf("\n\rR:13");
    for(col=0;col<u5DaisyChainSize;col++)
    {
        pc.printf(" %04x", (unsigned short)readAllReadback[0x1F][col]);
    }
    pc.printf("\n\r");
}
