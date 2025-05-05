#include "MAX17841_Interface.h"
#include "../../LowLevel/MAX17841/MAX17841.h"

#define CONFIGSIZE 3

extern Serial pc;
extern Timer timebase;

//The following configuration is being written into MAX17851-MASTER
WConfig M_17841_Conf[CONFIGSIZE] =  {
                            W_CONFIGURATION_1, 0x60,      //UART set to 2Mbps
                            W_CONFIGURATION_2,0x10,       //Set TX Queue
                            W_CONFIGURATION_3,0x05        //Set keep-alive for 160us
                            };

//The following configuration is being written into MAX17851-MASTER
WConfig S_17841_Conf[CONFIGSIZE] =  {
                            W_CONFIGURATION_1, 0x60,      //UART set to 2Mbps
                            W_CONFIGURATION_2,0x10,       //Set TX Queue
                            W_CONFIGURATION_3,0x05        //Set keep-alive for 160us
                            };

// SPI0 Master SPI Pins Initialization
// MOSI, MISO, SCK, SSEL
SPI spi0(P0_5, P0_6, P0_4, P0_7);

// SPI1 Slave SPI Pins Initialization
SPI spi1(P1_1, P1_2, P1_0, P1_3);

// Set SPI0 to the Master IC + Alert Pin
MAX17841 M_MAX17841(spi0, P3_3);
// Set SPI1 to the Slave IC + Alert Pin
MAX17841 S_MAX17841(spi1, P3_2);

DigitalOut dbg(P1_4);

int itr;
unsigned long localStartTime_us;

char MAX17841_Init(char *u5DaisySize)
{
    unsigned long ulWakeuptime;

    spi0.frequency(2000000);
    spi1.frequency(2000000);

    MAX17841_writeConfig('M',M_17841_Conf, CONFIGSIZE);   //Write MASTER Config
    //MAX17841_writeConfig('S',S_17841_Conf, CONFIGSIZE);   //Write SLAVE Config

    if(M_MAX17841.DC_wakeup(&ulWakeuptime))  //In case of Error:
    {
        pc.printf("No Device wakeup withing %d us\r\n", ulWakeuptime);
        pc.printf("Please check communication\r\n");
        return 1;
    }
    else
    {
        pc.printf("Chain woke up in %d us\r\n", ulWakeuptime);  //Only error messages printed here
    }

    if(M_MAX17841.DC_helloAll(0x00,u5DaisySize))
    {
        pc.printf("HELLOALL not returned!\nPlease check communication or Daisy Chain Supply\r\n");
        return 1;
    }
    else { }
    timebase.reset();
    timebase.start();

    return 0;
}

char MAX17841_readConfig(char MS, RConfig *ConfigRegs, char NumRegs)
{
    if(MS == 'M')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            M_MAX17841.ReadBytes(ConfigRegs[itr].ReadReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else if (MS == 'S')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            S_MAX17841.WriteBytes(ConfigRegs[itr].ReadReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

char MAX17841_writeConfig(char MS, WConfig* ConfigRegs, char NumRegs)
{
    if(MS == 'M')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            M_MAX17841.WriteBytes(ConfigRegs[itr].WriteReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else if (MS == 'S')
    {
        for (itr = 0 ; itr<NumRegs ; itr++) 
        {
            S_MAX17841.WriteBytes(ConfigRegs[itr].WriteReg,&ConfigRegs[itr].RegValue,1);
        }
        return 0;
    }
    else {return 1;}
}


char MAX17823_writeAll(char MS, char reg, unsigned short regval, unsigned int blockingTimeout_us)
{
    char returnval;

    if (MS == 'M')
    {
        M_MAX17841.DC_writeAll((unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = timebase.read_us();                               //Set timer to current ticker after Message has been delivered (only UART time)
        dbg = 1;
        while((timebase.read_us()-localStartTime_us) < blockingTimeout_us)    //Wait until timeout...
        {
            returnval = M_MAX17841.DC_writeAll_RB();         //...or successful transmission (2 means busy)
            if (returnval != 2) break;
        }
        dbg = 0;
        return returnval;
    }
    else if (MS == 'S')
    {
        S_MAX17841.DC_writeAll((unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = timebase.read_us(); 
        while((timebase.read_us()-localStartTime_us) < blockingTimeout_us)  //Wait until timeout...
        {
            returnval = S_MAX17841.DC_writeAll_RB();         //...or successful transmission (2 means busy)
            if (returnval != 2) break;
        }
        return returnval;
    } 
    else
    {
        return 1;   //Error in default
    }
}

char MAX17823_writeAll_RB(char MS)
{
    if (MS == 'M'){return M_MAX17841.DC_writeAll_RB();}
    else if (MS == 'S'){return S_MAX17841.DC_writeAll_RB();}
    else {return 1;}
}

char MAX17823_writeDevice(char MS, char u5Device, char reg, unsigned short regval, unsigned int blockingTimeout_us)
{
    char ret;

    if (MS == 'M')
    {
        M_MAX17841.DC_writeDevice(u5Device, (unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = timebase.read_us();                               //Set timer to current ticker after Message has been delivered (only UART time)
        dbg = 1;
        while((timebase.read_us()-localStartTime_us) < blockingTimeout_us)    //Wait until timeout...
        {
            ret = M_MAX17841.DC_writeDevice_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        dbg = 0;
        return ret;
    }
    else if (MS == 'S')
    {
        S_MAX17841.DC_writeDevice(u5Device, (unsigned char)reg,regval);     //Transmit Message
        localStartTime_us = timebase.read_us(); 
        while((timebase.read_us()-localStartTime_us) < blockingTimeout_us)  //Wait until timeout...
        {
            ret = S_MAX17841.DC_writeDevice_RB();         //...or successful transmission (2 means busy)
            if (ret != 2) break;
        }
        return ret;
    } 
    else
    {
        return 1;   //Error in default
    }
}

char MAX17823_writeDevice_RB(char MS)
{
    if (MS == 'M'){return M_MAX17841.DC_writeDevice_RB();}
    else if (MS == 'S'){return S_MAX17841.DC_writeDevice_RB();}
    else {return 1;}
}


char MAX17823_readAll(char MS, char reg)
{
    if (MS == 'M')
    {
        return M_MAX17841.DC_readAll((unsigned char) reg);
    }
    else if (MS == 'S')
    {
        return S_MAX17841.DC_readAll((unsigned char) reg);
    }
    else {return 1;}
}


char MAX17823_readAll_RB(char MS, unsigned short* values)
{
    if(MS == 'M'){return M_MAX17841.DC_readAll_RB(values);}
    else if(MS == 'S'){return S_MAX17841.DC_readAll_RB(values);}
    else{return 1;}
}

