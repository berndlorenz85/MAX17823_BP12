#include "mbed.h"
#include "../DataTypes.h"
#include "Scheduler.h"
#include "MAX77650_PowerCheck/MAX77650_PowerCheck.h"
#include "MAX17055_BatteryCheck/MAX17055_BatteryCheck.h"
#include "MAX17851_Interface/MAX17851_Interface.h"
#include "LTC2309_Measure.h"

#define MINBALVOLT  0xA3D0  //3.2V
#define CELLOVREG   0xD700  //4.199V
#define CELLUVREG   0x9C00  //3.047V
#define CELLMSMTCHREG   0x0084  //10mV
#define BBOVREG 0x8290  //+100mV for Bus Bar voltage alert
#define BBUVREG 0x7D70  //-100mV for Bus Bar voltage alert

extern Serial pc;

DigitalOut dbg2(P1_5);

// MCU board status LEDs
DigitalOut rLED(LED1);
DigitalOut gLED(LED2);
DigitalOut bLED(LED3);

#define RGBLED_WHITE    rLED = LED_ON;    bLED = LED_ON;    gLED = LED_ON
#define RGBLED_RED      rLED = LED_ON;    bLED = LED_OFF;    gLED = LED_OFF
#define RGBLED_GREEN    rLED = LED_OFF;   bLED = LED_OFF;    gLED = LED_ON
#define RGBLED_BLUE     rLED = LED_OFF;   bLED = LED_ON;    gLED = LED_OFF
#define RGBLED_PURPLE   rLED = LED_ON;    bLED = LED_ON;    gLED = LED_OFF

Timer scheduler;

int oldtick_us;
int oldtick_ms;
unsigned int innerLoop;
unsigned int slowLoop;

bool PowerCheckReady;
bool BatteryCheckReady;

bool innerLoopMiss = true;
bool looper = false;

int i32Temp, i32Print;
short i16Tempreg[8];
short i16Temp, i16Print;
int i32Tempreg[8], i32Tempreg2[8];  //Maximum 8 devices allowed
char* p8SourceAddress;
char* p8PrintAddress;
int* p32PrintAddress;   //pointer to print hex data
int* p32SourceAddress;  //pointer for source data for printing buffer

//Wakeup Variables
char u5DaisyChainSize = 0;  //Size of the daisy Chain (will be written at HELLOALL)
unsigned long waketime=0;   //Time the chain takes to wake up (in us)

char uvovdetected;

//Initializing for a maximum chain size of 8, the second dimension is for multiple scans per period.
DeviceData CellData[8];

short ConfigData[50];

char Scheduler_Init(void)
{  
    int i, j;

    //Initialization: WHITE
    RGBLED_WHITE;

    printf("Sizeof DeviceData: %d u16Values\r\n", sizeof(DeviceData)/2);    //Reserves LONG INT for every struct entry, SIZEOF gives bytes
    printf("Sizeof CellData: %d u16Values\r\n", sizeof(CellData)/2);        //Reserves LONG INT for every struct entry, SIZEOF gives bytes
    scheduler.start();  //Needs to be first because it's required by timeout of writeAll
    // Enable PMIC push button interrupt
    //MAX77650_PowerCheck_Init();

    //MAX17055_BatteryCheck_Init();

    if(MAX17851_Init(&waketime, &u5DaisyChainSize) != 0x00)
    {
        printf("Daisy Chain Initialization failed\r\n");
        return 1;
    }
    else {
        printf("Chain woke up in %u Microseconds\r\n", waketime);
        printf("MAX17851 Initialized. Chain Size = %d\r\n", u5DaisyChainSize);
        MAX17852_writeAll('M', DEVCFG1, 0xC109, 100);    //DUAL UART, UART HOST, DC Enable, DBL Buffer
        MAX17852_writeAll('M', STATUS1, 0x0000, 100);    //Clear ALRTRESET
        MAX17852_writeAll('M', STATUS2, 0x0000, 100);    //Clear Status
        MAX17852_writeAll('M', STATUS3, 0x0000, 100);    //Clear Status
        MAX17852_writeAll('M', AUXGPIOCFG, 0x8300, 100); //Enable I2C & AUX 2/3
        MAX17852_writeAll('M', PACKCFG, 0x0FFF, 100);    //Disable Flexpack
        MAX17852_writeAll('M', ALRTOVEN, 0x3FFF, 100);   //Enable all OV Alerts
        MAX17852_writeAll('M', ALRTUVEN, 0x3FFF, 100);   //Enable all UV Alerts
        MAX17852_writeAll('M', ALRTAUXOVEN, 0x000C, 100);  //Enable OVALRT for AUX2/3
        MAX17852_writeAll('M', OVTHCLRREG, CELLOVREG, 100);   //Set Overvoltage Alert to 3.45V (B0A4) (VOLT / 5 * 65536) [15:2]
        MAX17852_writeAll('M', OVTHSETREG, CELLOVREG, 100);   //Set Overvoltage Alert to 3.45V (B0A4) (VOLT / 5 * 65536) [15:2]
        MAX17852_writeAll('M', UVTHCLRREG, CELLUVREG, 100);   //Set Undervoltage Alert to 2.8V (8F5C) (VOLT / 5 * 65536) [15:2]
        MAX17852_writeAll('M', UVTHSETREG, CELLUVREG, 100);   //Set Undervoltage Alert to 2.8V (8F5C) (VOLT / 5 * 65536) [15:2]
        MAX17852_writeAll('M', MSMTCHREG, CELLMSMTCHREG, 100);      //Set Mismatch Alert for cells to 10mV (VOLT / 5 * 65536) [15:2] - flag to start balancing
        MAX17852_writeAll('M', BIPOVTHCLRREG, BBOVREG, 100);  //Overvoltage for BusBar = +100mV (VOLT / 5 * 65536 + 0x8000) [15:2]
        MAX17852_writeAll('M', BIPOVTHSETREG, BBOVREG, 100);  //Overvoltage for BusBar = +100mV (VOLT / 5 * 65536 + 0x8000) [15:2]
        MAX17852_writeAll('M', BIPUVTHCLRREG, BBUVREG, 100);  //Overvoltage for BusBar = -100mV (VOLT / 5 * 65536 + 0x8000) [15:2]
        MAX17852_writeAll('M', BIPUVTHSETREG, BBUVREG, 100);  //Overvoltage for BusBar = -100mV (VOLT / 5 * 65536 + 0x8000) [15:2]
        MAX17852_writeAll('M', CSAOVTHCLRREG, 0xFFFC, 100);  //Overcurrent Threshold Register - OFF
        MAX17852_writeAll('M', CSAOVTHSETREG, 0xFFFC, 100);  //Overcurrent Threshold Register - OFF
        MAX17852_writeAll('M', CSAUVTHCLRREG, 0x0000, 100);  //Overcurrent Threshold Register - OFF
        MAX17852_writeAll('M', CSAUVTHSETREG, 0x0000, 100);  //Overcurrent Threshold Register - OFF
        MAX17852_writeAll('M', AUXROVTHCLRREG, 0xFFFC, 100);  //Aux Ratiometric Overvoltage Threshold Register - OFF
        MAX17852_writeAll('M', AUXROVTHSETREG, 0xFFFC, 100);  //Aux Ratiometric Overvoltage Threshold Register - OFF
        MAX17852_writeAll('M', AUXRUVTHCLRREG, 0x0000, 100);  //Aux Ratiometric Undervoltage Threshold Register - OFF
        MAX17852_writeAll('M', AUXRUVTHSETREG, 0x0000, 100);  //Aux Ratiometric Undervoltage Threshold Register - OFF
        MAX17852_writeAll('M', COMPOVTHREG, CELLOVREG, 100);  //Comparator OV Threshold (CELL)
        MAX17852_writeAll('M', COMPUVTHREG, CELLUVREG, 100);  //Comparator UV Threshold (CELL)
        MAX17852_writeAll('M', COMPAUXROVTHREG, 0xFFC0, 100);  //Aux Ratiometric Undervoltage Threshold Register - OFF
        MAX17852_writeAll('M', COMPAUXRUVTHREG, 0x0000, 100);  //Aux Ratiometric Undervoltage Threshold Register - OFF
        MAX17852_writeAll('M', POLARITYCTRL, 0x0000, 100);   //All cells are unipolar and only unipolar will be considered for Mismatch
        MAX17852_writeAll('M', AUXREFCTRL, 0x0000, 100);  //VTHRM is reference to all AUX channels
        MAX17852_writeAll('M', AUXTIMEREG, 0x00A6, 100);  //Add some time for AUX to settle (HEX x 6us)
        MAX17852_writeAll('M', ACQCFG, 0x1000, 100);     //FOSR = 50Hz, Gain = 4
        MAX17852_writeAll('M', BALSWDLY, 0x0A0A, 100);     //Delay for AutoBalSwDis (96us x value) (for CELL and AUX)
        MAX17852_writeAll('M', MEASUREEN1, 0x3FFF, 100);  //Enable All Cells but no block
        MAX17852_writeAll('M', MEASUREEN2, 0x000C, 100);  //enable AUX2/3
        MAX17852_writeAll('M', BALEXP1, 0x0020, 100);         //max. 32s of balancing (in case of a fault or disconnect)
        MAX17852_writeAll('M', I2CCFG, 0xA000, 100);  //400k, Normal Format

        //First read to fill double buffer (start with last tick of ALTMUX scan)
        MAX17852_writeAll('M', DIAGCFG, 0x00AB, 100);    //DIAG1 = ADC Zero Scale, DIAG2 = ADC Full Scale
        MAX17852_writeAll('M', SCANCTRL, 0x105D, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8, ALTMUX

        //Wait until SCAN finishes (time to read out configuration)
        for(j=0;j<u5DaisyChainSize;j++)
        {
            MAX17852_readBlock('M', 0, 0x14, 25);   //Read Config Register 0x14 - 0x2C
            MAX17852_readBlock('S', 0, 0x2D, 25);   //Read Config Register 0x2D - 0x45
            wait_us(1000);
            MAX17852_readBlock_RB('M', &ConfigData[0]);   //Transfer data to buffer of first device
            MAX17852_readBlock_RB('S', &ConfigData[25]);   //Transfer data to buffer of second device
            printf("BMSIC Config %d: ", j);
            for(i=0;i<50;i++)
            {
                printf("%04x ", ConfigData[i]);
            }
            printf("\r\n");
        }
        
        //Initialization Done OK: GREEN
        RGBLED_GREEN;

        printf("Scheduler Start\r\n");
        scheduler.reset();
        oldtick_us = scheduler.read_us();
        oldtick_ms = scheduler.read_ms();
        innerLoop = 0;
        slowLoop = 0;
        return 0;
    }
}

void Scheduler_Loop(void)
{
    int i,j;
    if((scheduler.read_us() - oldtick_us) > 1000)   //1ms has passed
    {
        if(dbg2)    //Ticker output
        {
            dbg2 = 0;
        }
        else
        {
            dbg2 = 1;
        }

        if(innerLoopMiss)   //If we haven't gone here even once within 1ms...we missed a tick!!!
        {
            printf("Tick # %d missed by %d us\r\n", innerLoop, scheduler.read_us()-oldtick_us-1000);
        }
        oldtick_us = scheduler.read_us();
        switch(innerLoop)
        {
            case SCAN1:     //Check if previous SCAN has finished and then SCAN
                MAX17852_readAll('M', SCANCTRL);
                wait_us(200);
                MAX17852_readAll_RB('M', i16Tempreg);
                i16Temp = 0xA000;                   //Check for this pattern (if anyone doesn't have it, we're not there)
                for(j=0;j<u5DaisyChainSize;j++)     //READBLOCK ONLY FOR 2 DEVICES (add more case structures for higher chain sizes)
                {
                    i16Temp = i16Temp & i16Tempreg[j]; //Check for SCANDONE & DATARDY of device 0
                }
                //if(i16Temp == 0xA000)   //If all of them have both bits set, we move on
                //{
                    MAX17852_writeAll('M', DIAGCFG, 0x0012, 100);    //DIAG1 = Die Temp, DIAG2 = VAA
                    MAX17852_writeAll('M', SCANCTRL, 0x1059, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8
                //}
                //printf("firstTemp: %04x", i16Temp);
                break;
            case READBACK0_CELL_TICK1:
                MAX17852_readBlock('M', 0, CSAREG, 25); //Start readout of device 0 from Master interface
                MAX17852_readBlock('S', 1, CSAREG, 25); //Start readout of device 1 from Slave interface
                break;
            case READBACK0_CELL_TICK2:
                MAX17852_readBlock_RB('M', (short*)&CellData[0]);   //Transfer data to buffer of first device
                MAX17852_readBlock_RB('S', (short*)&CellData[1]);   //Transfer data to buffer of second device
                break;
            case READBACK0_STATUS_TICK1:
                MAX17852_readBlock('M', 0, VERSION, 19);    //Read All status bits of device 0
                MAX17852_readBlock('S', 1, VERSION, 19);    //Read All status bits of device 1
                break;
            case READBACK0_STATUS_TICK2:    //Readback of Readblock (16 + 2x BLKSIZE characters = 62) -> 62x12 = 792bits -> 396us per device -> 8 Devices = 3.17ms -> ADD 4 ticks
                MAX17852_readBlock_RB('M', (short*)&CellData[0].statusvalues);   //Transfer STATUS data to buffer of device 0
                MAX17852_readBlock_RB('S', (short*)&CellData[1].statusvalues);   //Transfer STATUS data to buffer of device 1
                break;
            case SM59_CHECK_RESULTS:            //Last step was SM59 ALTMUX Scan
                if((i16Temp & 0xA000) == 0xA000)           //only if all of them have passed, we move on
                {
                    CellData[j].SM59[0] = CellData[j].statusvalues[0x07] & 0x5000;  //mask with COMPOVST and COMPUVST in register ALRTSUM (0x07)
                }
                else    //SM59 is set to FFFF (invalid) as measurement didn't finish
                {
                    CellData[j].SM59[0] = 0xFFFF;
                }
                MAX17852_writeAll('M', COMPOVTHREG, CELLOVREG, 100);   //Reset OV Threshold for comparator
                MAX17852_writeAll('M', COMPUVTHREG, CELLUVREG, 100);   //Reset UV Threshold for comparator
                break;
            case PRINT1_0:        //Transmit data to HOST (splitting up prints in 6x1ms chunks) - currently only 1 device supported
                p8PrintAddress = (char*)&CellData[0];   //Set starting pointer (currently only 1 device supported)
                printf("$$\r\n");                       //start string
                i32Print = 0;
                break;
            case PRINT1_1:
            case PRINT1_2:
            case PRINT1_3:
            case PRINT1_4:
            case PRINT1_5:
            case PRINT1_6:
            case PRINT1_7:
            case PRINT1_8:
            case PRINT1_9:
                while(i32Print<sizeof(DeviceData))
                {
                    pc.putc(*(p8PrintAddress+i32Print));
                    i32Print++;
                    if(i32Print%15 == 0) break;   //break after 20 bytes
                }
                break;
//--------------------------------------
            case SCAN2:
                MAX17852_readAll('M', SCANCTRL);
                wait_us(200);
                MAX17852_readAll_RB('M', i16Tempreg);
                i16Temp = 0xA000;                   //Check for this pattern (if anyone doesn't have it, we're not there)
                for(j=0;j<u5DaisyChainSize;j++)     //READBLOCK ONLY FOR 2 DEVICES (add more case structures for higher chain sizes)
                {
                    i16Temp = i16Temp & i16Tempreg[j]; //Check for SCANDONE & DATARDY of device 0
                }
                if((i16Temp & 0xA000) == 0xA000)   //If all of them have both bits set, we move on
                {
                    MAX17852_writeAll('M', DIAGCFG, 0x0034, 100);    //DIAG1 = CellSignalPath, DIAG2 = Comp Signal Path
                    MAX17852_writeAll('M', SCANCTRL, 0x1059, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8
                }
                break;
            case READBACK1_CELL_TICK1:
                MAX17852_readBlock('M', 0, CSAREG, 25); //Start readout of device 0 from Master interface
                MAX17852_readBlock('S', 1, CSAREG, 25); //Start readout of device 1 from Slave interface
                break;
            case READBACK1_CELL_TICK2:
                MAX17852_readBlock_RB('M', (short*)&CellData[0]);   //Transfer data to buffer of first device
                MAX17852_readBlock_RB('S', (short*)&CellData[1]);   //Transfer data to buffer of second device
                break;
            case READBACK1_STATUS_TICK1:
                MAX17852_readBlock('M', 0, VERSION, 19);    //Read All status bits of device 0
                MAX17852_readBlock('S', 1, VERSION, 19);    //Read All status bits of device 1
                break;
            case READBACK1_STATUS_TICK2:    //Readback of Readblock (16 + 2x BLKSIZE characters = 62) -> 62x12 = 792bits -> 396us per device -> 8 Devices = 3.17ms -> ADD 4 ticks
                MAX17852_readBlock_RB('M', (short*)&CellData[0].statusvalues);   //Transfer STATUS data to buffer of device 0
                MAX17852_readBlock_RB('S', (short*)&CellData[1].statusvalues);   //Transfer STATUS data to buffer of device 1
                break;
           case CHECK_OVUVALERTS1:
                uvovdetected = 0;
                for (j=0;j<u5DaisyChainSize;j++)
                {
                    if(CellData[j].statusvalues[2] & 0x1800)    //if ALRTOVST or ALRTUVST
                    {
                        uvovdetected = 1;
                    }
                }
                if(uvovdetected)
                {
                    MAX17851_SetGPIO('M', 0x3);
                    MAX17851_SetGPIO('S', 0x3);
                }
                else
                {
                    MAX17851_SetGPIO('M', 0x0);
                    MAX17851_SetGPIO('S', 0x0);
                }
                break;
            case PRINT2_0:        //Transmit data to HOST (splitting up prints in 6x1ms chunks) - currently only 1 device supported
                p8PrintAddress = (char*)&CellData[0];   //Set starting pointer (currently only 1 device supported)
                //printf("$$\r\n");                       //start string
                i32Print = 0;
            case PRINT2_1:
            case PRINT2_2:
            case PRINT2_3:
            case PRINT2_4:
            case PRINT2_5:
            case PRINT2_6:
            case PRINT2_7:
            case PRINT2_8:
            case PRINT2_9:
                while(i32Print<sizeof(DeviceData))
                {
                    pc.putc(*(p8PrintAddress+i32Print));
                    i32Print++;
                    if(i32Print%15 == 0) break;   //break after 15 bytes
                }
                break;

//-------------------------------
            case SCAN3:
                MAX17852_readAll('M', SCANCTRL);
                wait_us(200);
                MAX17852_readAll_RB('M', i16Tempreg);
                i16Temp = 0xA000;                   //Check for this pattern (if anyone doesn't have it, we're not there)
                for(j=0;j<u5DaisyChainSize;j++)     //READBLOCK ONLY FOR 2 DEVICES (add more case structures for higher chain sizes)
                {
                    i16Temp = i16Temp & i16Tempreg[j]; //Check for SCANDONE & DATARDY of device 0
                }
                if((i16Temp & 0xA000) == 0xA000)   //If all of them have both bits set, we move on
                {
                    MAX17852_writeAll('M', DIAGCFG, 0x0078, 100);    //DIAG1 = 3/4-Scale DAC Test, DIAG2 = 1/4-Scale DAC Test
                    MAX17852_writeAll('M', SCANCTRL, 0x1059, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8
                }
                break;
            case READBACK2_CELL_TICK1:
                MAX17852_readBlock('M', 0, CSAREG, 25); //Start readout of device 0 from Master interface
                MAX17852_readBlock('S', 1, CSAREG, 25); //Start readout of device 1 from Slave interface
                break;
            case READBACK2_CELL_TICK2:
                MAX17852_readBlock_RB('M', (short*)&CellData[0]);   //Transfer data to buffer of first device
                MAX17852_readBlock_RB('S', (short*)&CellData[1]);   //Transfer data to buffer of second device
                break;
            case READBACK2_STATUS_TICK1:
                MAX17852_readBlock('M', 0, VERSION, 19);    //Read All status bits of device 0
                MAX17852_readBlock('S', 1, VERSION, 19);    //Read All status bits of device 1
                break;
            case READBACK2_STATUS_TICK2:    //Readback of Readblock (16 + 2x BLKSIZE characters = 62) -> 62x12 = 792bits -> 396us per device -> 8 Devices = 3.17ms -> ADD 4 ticks
                MAX17852_readBlock_RB('M', (short*)&CellData[0].statusvalues);   //Transfer STATUS data to buffer of device 0
                MAX17852_readBlock_RB('S', (short*)&CellData[1].statusvalues);   //Transfer STATUS data to buffer of device 1
                break;

            case BALANCING_TICK1:    //Check Max Cell and determine balancing
                MAX17852_readBlock('M', 0, BALSTAT, 2);
                MAX17852_readBlock('S', 0, BALSTAT, 2);        
                break;
            case BALANCING_TICK2:
                MAX17852_readBlock_RB('M', (short*)&CellData[0].Balancing[1]);
                MAX17852_readBlock_RB('S', (short*)&CellData[1].Balancing[1]);
                
                for (j=0;j<u5DaisyChainSize;j++)
                {
                    switch(CellData[j].Balancing[1] & 0xC000)     //Check Balancing Status
                    {
                        case 0x0000:    //Balancing disabled
                            if(CellData[j].statusvalues[2] & 0x2000)    //ALRTMSMTCH (MAX-MIN > MISMATCH register)
                            {
                                //Delta is bigger than 10mV: Balance MAXCELL, BLUE
                                CellData[j].Balancing[0] = 0x0001<<((CellData[j].statusvalues[0x0A] >> 8)-1);   //Write BALSWCTRL to the MAXCELL value
                                MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Set Balancing Switch
                                MAX17852_writeDevice('M', j, BALCTRL, 0x10F0, 100);                             //Enable Manual Balancing by second
                                RGBLED_BLUE;
                            }
                            else 
                            {
                                //Mismatch is OK
                                RGBLED_GREEN;
                            }
                            break;
                        case 0x4000:    //Balancing active
                            if(CellData[j].statusvalues[2] & 0x2000)    //Cells are still unbalanced
                            {
                                CellData[j].Balancing[0] = 0x0001<<((CellData[j].statusvalues[0x0A] >> 8)-1);   //Write BALSWCTRL to the MAXCELL value (may be a different one)
                                MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Set Balancing Switch according to MAXCELL
                                RGBLED_BLUE;
                            }
                            else
                            {
                                CellData[j].Balancing[0] = 0;
                                MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Disable all balancing switches
                                MAX17852_writeDevice('M', j, BALCTRL, 0x00F0, 100);                             //Disable Balancing (nothing to do here)
                                RGBLED_GREEN;
                            }
                            break;
                        case 0x8000:    //Balancing stopped
                            if(CellData[j].statusvalues[2] & 0x2000)    //Cells are still unbalanced, restart
                            {
                                CellData[j].Balancing[0] = 0x0001<<((CellData[j].statusvalues[0x0A] >> 8)-1);   //Write BALSWCTRL to the MAXCELL value (may be a different one)
                                MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Set Balancing Switch according to MAXCELL
                                MAX17852_writeDevice('M', j, BALCTRL, 0x10F0, 100);                             //Enable Manual Balancing by second
                                RGBLED_BLUE;
                            }
                            else 
                            {
                                CellData[j].Balancing[0] = 0;
                                MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Set Balancing Switch OFF
                                MAX17852_writeDevice('M', j, BALCTRL, 0x00F0, 100);   //Disable Balancing (everything balanced)
                                RGBLED_GREEN;
                            }
                            break;
                        case 0xC000:    //Balancing Halted
                            CellData[j].Balancing[0] = 0;
                            MAX17852_writeDevice('M', j, BALSWCTRL, CellData[j].Balancing[0], 100);         //Set Balancing Switch OFF
                            MAX17852_writeDevice('M', j, BALCTRL, 0x00F0, 100);   //Disable Balancing (need to cool down)
                            RGBLED_RED;
                            break;
                        default:
                            break;
                    }
                }
                break;
            case PRINT3_0:        //Transmit data to HOST (splitting up prints in 6x1ms chunks) - currently only 1 device supported
                p8PrintAddress = (char*)&CellData[0];   //Set starting pointer (currently only 1 device supported)
                i32Print = 0;
            case PRINT3_1:
            case PRINT3_2:
            case PRINT3_3:
            case PRINT3_4:
            case PRINT3_5:
            case PRINT3_6:
            case PRINT3_7:
            case PRINT3_8:
            case PRINT3_9:
                while(i32Print<sizeof(DeviceData))
                {
                    pc.putc(*(p8PrintAddress+i32Print));
                    i32Print++;
                    if(i32Print%15 == 0) break;   //break after 20 bytes
                }
                break;

//----------------------------------
            case SM59_PREP:     //Writing OV and UV for each device separately
                for (j=0;j<u5DaisyChainSize;j++)
                {
                    if((CellData[j].statusvalues[0x0A] >> 8) && (CellData[j].statusvalues[0x0A] & 0x00FF))  //Checking if both are non-zero (index goes from 1-F) to avoid memory violation due to "-1" on indexing
                    {
                        //Set comparator OV detection to a minimum of 10mV (max. 20mV) above adc limit
                        CellData[j].SM59[1] = (CellData[j].cellvalues[(CellData[j].statusvalues[0x0A] >> 8)-1] & 0xFFC0) + 0x0200;
                        MAX17852_writeDevice('M', j, COMPOVTHREG, CellData[j].SM59[1], 100);
                        //Set Comparator UV detection to a minimum 10mV (max. 20mV) below ADC limit
                        CellData[j].SM59[2] = (CellData[j].cellvalues[(CellData[j].statusvalues[0x0A] & 0x00FF)-1] & 0xFFC0) - 0x0100;
                        MAX17852_writeDevice('M', j, COMPUVTHREG, CellData[j].SM59[2], 100);
                    }
                }
                break;
            case SCAN_ALTMUX:
                MAX17852_readAll('M', SCANCTRL);
                wait_us(200);
                MAX17852_readAll_RB('M', i16Tempreg);
                i16Temp = 0xA000;                   //Check for this pattern (if anyone doesn't have it, we're not there)
                for(j=0;j<u5DaisyChainSize;j++)     //READBLOCK ONLY FOR 2 DEVICES (add more case structures for higher chain sizes)
                {
                    i16Temp = i16Temp & i16Tempreg[j]; //Check for SCANDONE & DATARDY of device 0
                }
                if((i16Temp & 0xA000) == 0xA000)   //If all of them have both bits set, we move on
                {
                    MAX17852_writeAll('M', DIAGCFG, 0x00AB, 100);    //DIAG1 = ADC Zero Scale, DIAG2 = ADC Full Scale
                    MAX17852_writeAll('M', SCANCTRL, 0x105D, 100);   //SCAN: AUTOBALSWDIS, ADC+COMP, OS8, ALTMUX
                }
                break;
            case READBACK3_CELL_TICK1:
                MAX17852_readBlock('M', 0, CSAREG, 25); //Start readout of device 0 from Master interface
                MAX17852_readBlock('S', 1, CSAREG, 25); //Start readout of device 1 from Slave interface
                break;
            case READBACK3_CELL_TICK2:
                MAX17852_readBlock_RB('M', (short*)&CellData[0]);   //Transfer data to buffer of first device
                MAX17852_readBlock_RB('S', (short*)&CellData[1]);   //Transfer data to buffer of second device
                break;
            case READBACK3_STATUS_TICK1:
                MAX17852_readBlock('M', 0, VERSION, 19);    //Read All status bits of device 0
                MAX17852_readBlock('S', 1, VERSION, 19);    //Read All status bits of device 1
                break;
            case READBACK3_STATUS_TICK2:    //Readback of Readblock (16 + 2x BLKSIZE characters = 62) -> 62x12 = 792bits -> 396us per device -> 8 Devices = 3.17ms -> ADD 4 ticks
                MAX17852_readBlock_RB('M', (short*)&CellData[0].statusvalues);   //Transfer STATUS data to buffer of device 0
                MAX17852_readBlock_RB('S', (short*)&CellData[1].statusvalues);   //Transfer STATUS data to buffer of device 1
                break;
/* Temporarily disabled
            case 60:
                if(BatteryCheckReady)
                {
                    BatteryCheckReady=false;
                    //MAX17055_BatteryCheck_Loop();
                }
                break;
            case 65:
                if(PowerCheckReady)
                {
                    PowerCheckReady=false;
                    //MAX77650_PowerCheck_Loop();
                }
                break;

            case COPY_DATA_TO_SEND:
                for(j=0;j<u5DaisyChainSize;j++)    //For 2 devices
                {
                    p8SourceAddress = (char*)&CellData[j];
                    p8PrintAddress = (char*)&SendData[j];
                    for(i=0;i<(sizeof(DeviceData));i++)
                    {
                        *(p8PrintAddress+i) = *(p8SourceAddress+i);
                    }
                }
                break;
*/            
            case PRINT4_0:        //Transmit data to HOST (splitting up prints in 6x1ms chunks) - currently only 1 device supported
                p8PrintAddress = (char*)&CellData[0];   //Set starting pointer (currently only 1 device supported)
                //printf("$$\r\n");                       //start string
                i32Print = 0;
            case PRINT4_1:
            case PRINT4_2:
            case PRINT4_3:
            case PRINT4_4:
            case PRINT4_5:
            case PRINT4_6:
            case PRINT4_7:
            case PRINT4_8:
            case PRINT4_9:
                while(i32Print<sizeof(DeviceData))
                {
                    pc.putc(*(p8PrintAddress+i32Print));
                    i32Print++;
                    if(i32Print%15 == 0) break;   //break after 20 bytes
                }
                break;
            
//-----------------------------------------------
            default:
                break;
        }
        if(innerLoop < LOOPTICKS_US) //inner loop finished
        {
            innerLoop++;
        }
        else    //If we ran through a full 100 states
        {
            if(pc.readable())   //User typed something
            {
                switch(pc.getc())
                {
                    case '1':   //restart from 0 after a loop
                        looper = true;
                        if (looper == false)
                        {
                            oldtick_us = scheduler.read_us();   //restart timer, otherwise we get an error flag
                        }else{}
                        break;
                    case '0':   //remain in 100th state until re-enabled
                        looper = false;
                        break;
                    default:
                        break;
                }
            } else {}

            if(looper)  //Reset to 0 if we are in "loop" mode
            {
                innerLoop=0;
            } else {}
        }
        innerLoopMiss = true;   //setting it to true for the next loop. If we didn't do at least ONE "else", case, we have missed a tick
    }
    else
    {
        innerLoopMiss = false;  //setting the flag to FALSE, meaning we have executed the scheduler loop at least once before going to the main SWITCH-CASE
    }
    
    if ((scheduler.read_ms() - oldtick_ms) > 100)  //100ms have passed
    {
        oldtick_ms = scheduler.read_ms();
        switch(slowLoop)
        {
            case 10:
                BatteryCheckReady = true;
                break;
            case 30:
                PowerCheckReady = true;
                break;
            default:
                break;
        }
        if (slowLoop < 100) //10s outter loop
            slowLoop++;
        else
            slowLoop = 0;
    } else {}
}
