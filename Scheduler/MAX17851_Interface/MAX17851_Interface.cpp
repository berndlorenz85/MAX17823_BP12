#include "MAX17851_Interface.h"
#include "../../LowLevel/MAX17851/MAX17851.h"

#define CONFIGSIZE 7

extern Timer scheduler; //Use timer to get accurate microsecond ticker for timeout

//The following configuration is being written into MAX17851-MASTER
WConfig M_17851_Conf[CONFIGSIZE] =  {
                            W_CONFIG_GEN0, 0x01,      //Set Daisy Chain Size to 1
                            W_CONFIG_GEN1,0x30,       //Set baud rate = 2Mbps
                            W_CONFIG_GEN3,0x05,       //Set keep alive rate
                            W_CONFIG_GEN4,0x38,       //Enable master dual UART with normal data sequence
                            W_CONFIG_GPIO12,0x66,     //Enabling GPIO1/2 MOSFET driver in SAFEMON (active HIGH)
                            W_CONFIG_GPIO34,0x46,     //GPIO4 = Micro Reset (one-shot high), GPIO3 = HIGH to put SLAVE in charge
                            W_ALERT_OPSTATE,0x00};    //Clear Alert Reset

//The following configuration is being written into MAX17851-Slave
WConfig S_17851_Conf[CONFIGSIZE] =  {
                            W_CONFIG_GEN0, 0x01,      //Set Daisy Chain Size to 1
                            W_CONFIG_GEN1,0x30,       //Set baud rate = 2Mbps
                            W_CONFIG_GEN3,0x05,       //Set keep alive to 160us
                            W_CONFIG_GEN4,0x48,       //Enable slave dual UART with RXSWAP Enabled
                            W_CONFIG_GPIO12,0x66,     //Enabling GPIO1/2 MOSFET driver in SAFEMON (active HIGH)
                            W_CONFIG_GPIO34,0x16,     //GPIO4 = SAFEMON Slave Input, GPIO3 = Pulse high to reset power supply to micro
                            W_ALERT_OPSTATE,0x00};    //Clear Alert Reset



// SPI0 Master SPI Pins Initialization
SPI spi0(P0_5, P0_6, P0_4, P0_7);

// SPI1 Slave SPI Pins Initialization
SPI spi1(P1_1, P1_2, P1_0, P1_3);

// Set SPI0 to the Master IC + Alert Pin
MAX17851 M_MAX17851(spi0, P3_3);
// Set SPI1 to the Slave IC + Alert Pin
MAX17851 S_MAX17851(spi1, P3_2);

DigitalOut dbg(P1_4);

int itr;
unsigned long localStartTime_us;

char MAX17851_Reset(char MS)
{
    if(MS == 'M')
    {
        M_MAX17851.WriteBytes(W_SWPOR);
    }
    else
    {
        S_MAX17851.WriteBytes(W_SWPOR);
    }
    wait_us(10);
    return 0;
}

char MAX17851_SetGPIO(char MS, char GPIO12)
{
    char writeCharGPIO12;

    if(GPIO12 & 0x02)   //GPIO2
    {
        writeCharGPIO12 = 0x20;
    }
    else {
        writeCharGPIO12 = 0x30;
    }
    if(GPIO12 & 0x01)    //GPIO1
    {
        writeCharGPIO12 += 0x02;
    }
    else
    {
        writeCharGPIO12 += 0x03;
    }

    if(MS == 'M')
    {
        M_MAX17851.WriteBytes(W_CONFIG_GPIO12, &writeCharGPIO12, 1);
    }
    else if (MS == 'S')
    {
        S_MAX17851.WriteBytes(W_CONFIG_GPIO12, &writeCharGPIO12, 1);
    }
    else {}
    return 0;
}

char MAX17851_Init(unsigned long *ulWakeuptime, char *u5DaisySize)
{
    char registerCheck[24]; //Temp register to write status register
    int i;

    spi0.frequency(2000000);
    spi1.frequency(2000000);
    MAX17851_Reset('M');
    MAX17851_Reset('S');

    MAX17851_writeConfig('M',M_17851_Conf, CONFIGSIZE);   //Write MASTER Config
    MAX17851_writeConfig('S',S_17851_Conf, CONFIGSIZE);   //Write SLAVE Config

    if(M_MAX17851.DC_wakeup(ulWakeuptime))  //In case of Error:
    {
        printf("No Device wakeup withing %d us\r\n", *ulWakeuptime);
        printf("Please check communication\r\n");
        return 1;
    }
    else
    {
        //printf("Chain woke up in %d us\r\n", *ulWakeuptime);  //Only error messages printed here
    }

    if(M_MAX17851.DC_helloAll(0x00,u5DaisySize))
    {
        printf("HELLOALL not returned!\nPlease check communication or Daisy Chain Supply\r\n");
        return 1;
    }
    else 
    {
        M_MAX17851.WriteDaisyChainParameters(*u5DaisySize);
        S_MAX17851.WriteDaisyChainParameters(*u5DaisySize);
    }

    //Read out Configuration of SPI2UART Device
    //MASTER
    M_MAX17851.ReadBytes(0x61,registerCheck,21);
    printf("SPI2UART Config 0: ");
    for(i=0;i<21;i++)
    {
        printf("%02x ", registerCheck[i]);
    }
    printf("\r\n");
    //SLAVE
    S_MAX17851.ReadBytes(0x61,registerCheck,21);
    printf("SPI2UART Config 1: ");
    for(i=0;i<21;i++)
    {
        printf("%02x ", registerCheck[i]);
    }
    printf("\r\n");

    if(M_MAX17851.ReadAlertPinB() == false) //It never gets false...need to check
    {
        M_MAX17851.ReadBytes(0x01,registerCheck,24);
        printf("Alert on Interface! Status Reg 0x01: ");
        for(i=0;i<24;i++)
        {
            printf("%02x ", registerCheck[i]);
        }
        printf("\r\n");
        return 1;
    }
    else 
    {
        return 0;
    }
}

char MAX17851_readConfig(char MS, RConfig *ConfigRegs, char NumRegs)
{
    if(MS == 'M')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            M_MAX17851.ReadBytes(ConfigRegs[itr].ReadReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else if (MS == 'S')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            S_MAX17851.WriteBytes(ConfigRegs[itr].ReadReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

char MAX17851_writeConfig(char MS, WConfig* ConfigRegs, char NumRegs)
{
    if(MS == 'M')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            M_MAX17851.WriteBytes(ConfigRegs[itr].WriteReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else if (MS == 'S')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            S_MAX17851.WriteBytes(ConfigRegs[itr].WriteReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else {return 1;}
}


char MAX17852_writeAll(char MS, char reg, short regval, unsigned int blockingTimeout_us)
{
    char ret;

    if (MS == 'M')
    {
        M_MAX17851.DC_writeAll((unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = scheduler.read_us();                               //Set timer to current ticker after Message has been delivered (only UART time)
        dbg = 1;
        while((scheduler.read_us()-localStartTime_us) < blockingTimeout_us)    //Wait until timeout...
        {
            ret = M_MAX17851.DC_writeAll_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        dbg = 0;
        return ret;
    }
    else if (MS == 'S')
    {
        S_MAX17851.DC_writeAll((unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = scheduler.read_us(); 
        while((scheduler.read_us()-localStartTime_us) < blockingTimeout_us)  //Wait until timeout...
        {
            ret = S_MAX17851.DC_writeAll_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        return ret;
    } 
    else
    {
        return 1;   //Error in default
    }
}

char MAX17852_writeAll_RB(char MS)
{
    if (MS == 'M'){return M_MAX17851.DC_writeAll_RB();}
    else if (MS == 'S'){return S_MAX17851.DC_writeAll_RB();}
    else {return 1;}
}

char MAX17852_writeDevice(char MS, char u5Device, char reg, short regval, unsigned int blockingTimeout_us)
{
    char ret;

    if (MS == 'M')
    {
        M_MAX17851.DC_writeDevice(u5Device, (unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = scheduler.read_us();                               //Set timer to current ticker after Message has been delivered (only UART time)
        dbg = 1;
        while((scheduler.read_us()-localStartTime_us) < blockingTimeout_us)    //Wait until timeout...
        {
            ret = M_MAX17851.DC_writeDevice_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        dbg = 0;
        return ret;
    }
    else if (MS == 'S')
    {
        S_MAX17851.DC_writeDevice(u5Device, (unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = scheduler.read_us(); 
        while((scheduler.read_us()-localStartTime_us) < blockingTimeout_us)  //Wait until timeout...
        {
            ret = S_MAX17851.DC_writeDevice_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        return ret;
    } 
    else
    {
        return 1;   //Error in default
    }
}

char MAX17852_writeDevice_RB(char MS)
{
    if (MS == 'M'){return M_MAX17851.DC_writeDevice_RB();}
    else if (MS == 'S'){return S_MAX17851.DC_writeDevice_RB();}
    else {return 1;}
}


char MAX17852_readAll(char MS, char reg)
{
    if (MS == 'M')
    {
        return M_MAX17851.DC_readAll((unsigned char) reg);
    }
    else if (MS == 'S')
    {
        return S_MAX17851.DC_readAll((unsigned char) reg);
    }
    else {return 1;}
}


char MAX17852_readAll_RB(char MS, short* values)
{
    if(MS == 'M'){return M_MAX17851.DC_readAll_RB(values);}
    else if(MS == 'S'){return S_MAX17851.DC_readAll_RB(values);}
    else{return 1;}
}

char MAX17852_readBlock(char MS, char u5Device, char u8StartAddr, char u5BlockSize)
{
    if(MS == 'M')
    {
        return M_MAX17851.DC_readBlock(u5Device, u8StartAddr, u5BlockSize);
    }
    else if(MS == 'S')
    {
        return S_MAX17851.DC_readBlock(u5Device, u8StartAddr, u5BlockSize);
    }
    else{return 1;}
}

char MAX17852_readBlock_RB(char MS, short* values)
{
    if(MS == 'M'){return M_MAX17851.DC_readBlock_RB(values);}
    else if(MS == 'S'){return S_MAX17851.DC_readBlock_RB(values);}
    else{return 1;}
}

void MAX17851_Loop(void)
{
    //Do Command Execution
}
