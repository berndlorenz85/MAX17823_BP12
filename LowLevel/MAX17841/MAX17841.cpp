#include "MAX17841.h"
#include "mbed.h"
//#include "MAX32620FTHR.h"

extern Serial pc;
extern char u5DaisyChainSize;   //access the same variable as calling function

char DC_PEC(char *data, char len)
{
    const int polynomial = 0xB2;
    char remainder = 0;
    int crcBits, itr;

    for (itr=0;itr<len;itr++)
    {
        remainder ^= *(data+itr);
        for (crcBits=0;crcBits<8;crcBits++)
        {
            if (remainder & 0x01)
                remainder = ((remainder >> 1) ^ polynomial);
            else
                remainder = (remainder >> 1);
        }
    }
    return remainder;
}

//******************************************************************************
MAX17841::MAX17841(SPI &extspi, PinName AlertPin) :
myspi(extspi)  /*Assigning the externally defined SPI interface to internal SPI*/
{
    AlrtPin = new DigitalIn(AlertPin);
}

//******************************************************************************
MAX17841::~MAX17841()
{
    delete AlrtPin;     /*Release Alert pin*/
    delete &myspi;       /*Release SPI interface*/
}

//******************************************************************************

bool MAX17841::ReadAlertPinB(void)
{
    return AlrtPin;
}

char MAX17841::WriteBytes(char reg, char *writeValues, int NumDataBytes)
{
    writebuf[0] = reg;

    for (itr = 0 ; itr<NumDataBytes ; itr++) 
    {
        writebuf[itr + 1] = writeValues[itr];
    }

    myspi.write (writebuf, NumDataBytes+1, readbuf, NumDataBytes+1);

    return MAX17841_NO_ERROR;
}

char MAX17841::WriteBytes(char reg)
{
    writebuf[0] = reg;
    myspi.write (writebuf, 1, readbuf, 1);

    return MAX17841_NO_ERROR;
}

char MAX17841::ReadBytes(char reg, char *readValues, int NumDataBytes)
{
    writebuf[0] = reg;

    myspi.write (writebuf, NumDataBytes+1, readbuf, NumDataBytes+1);

    for (itr = 0 ; itr<NumDataBytes ; itr++) 
    {
        readValues[itr] = readbuf[itr+1];
    }

    return MAX17841_NO_ERROR;
}

char MAX17841::DC_wakeup(unsigned long *wakeuptime_us)
{
    char oldVal, tempval;
    char preambles = 0x30;
    char keepalive = 0x05;
    //M_MAX17841::MAX17841_RRegister_t preamblereg;
    //preamblereg = M_MAX17841.W_CONFIG_GEN2;

    WriteBytes(A_CLR_RX_BUF);
    WriteBytes(A_CLR_TX_BUF);
    WriteBytes(W_CONFIGURATION_3, &keepalive,1);    /*Set Keep Alives*/
    ReadBytes(R_CONFIGURATION_2, &oldVal, 1);       /*Read out old value*/
    WriteBytes(W_CONFIGURATION_2, &preambles,1);    /*Send continuous preambles*/
    for(itr=0;itr<100;itr++)
    {
        ReadBytes(R_STATUS_RX, &tempval, 1);        /*Read out RX register for received preambles*/
        if((tempval & 0x21) == 0x21)
        {
            break;
        }
        wait_us(100);
    }
    *wakeuptime_us = (unsigned long)itr * 100;
    WriteBytes(W_CONFIGURATION_2, &oldVal,1);      /*Restore old values*/
    
    if (itr >= 99)  //waited for longer than 10ms - something is wrong
    {
        return MAX17841_ERROR;
    }
    else
    {
        return MAX17841_NO_ERROR;
    }
}

char MAX17841::DC_helloAll(char seed, char *DCSize)
{
    char tempval;

    rwbuf[0] = 0x03;
    rwbuf[1] = 0x57;
    rwbuf[2] = 0x00;
    rwbuf[3] = seed;

    u5DaisyChainSize = seed;

    WriteBytes(A_CLR_RX_BUF);
    WriteBytes(A_CLR_TX_BUF);

    WriteBytes(A_WR_LD_Q, rwbuf, 4);
    WriteBytes(A_WR_NXT_LD_Q);

    for(itr=0;itr<100;itr++)
    {
        ReadBytes(R_STATUS_RX, &tempval, 1);   /*Read out RX register for received preambles*/
        if((tempval & 0x12) == 0x12)
        {               //RX Stop Received
            break;
        }
        wait_us(100);
    }
    if(itr < 10)
    {
        ReadBytes(A_RX_RD_NXT_MSG, rwbuf, 4);       //0x57, seed, return, PEC
        ReadBytes(R_STATUS_RX, &tempval, 1);        //Check if empty
        *DCSize = rwbuf[2]-seed;
        return MAX17841_NO_ERROR;
    }
    else
    {
        return MAX17841_ERROR;
    }
}

char MAX17841::DC_writeAll(char reg, unsigned short regval)
{
    rwbuf[0] = 0x05;                        //Length on UART
    rwbuf[1] = 0x02;                        //WRITEALL
    rwbuf[2] = (char)reg;                //Reg Address
    rwbuf[3] = (char)(regval & 0x00FF);  //LSB
    rwbuf[4] = (char)(regval >> 8);      //MSB
    rwbuf[5] = DC_PEC(&rwbuf[1],4);         //PEC

    WriteBytes(A_CLR_RX_BUF);
    WriteBytes(A_CLR_TX_BUF);

    WriteBytes(A_WR_LD_Q, rwbuf, 6);            //Transfer data to TX Buffer
    WriteBytes(A_WR_NXT_LD_Q);                  //Transmit TX Buffer

    //check if bus is still busy and return fault if it is
    return MAX17841_NO_ERROR;
}


char MAX17841::DC_writeAll_RB(void)
{
    char tempval, i;

    ReadBytes(R_STATUS_RX, &tempval, 1);            /*Read out RX register for received preambles*/

    if((tempval & 0x12) == 0x12)                    //RX Stop Received
    {
        ReadBytes(A_RX_RD_NXT_MSG, rwbuf, 5);       //Read RX Buffer (+1 for ALIVECTR)
        ReadBytes(R_STATUS_RX, &tempval, 1);        /*Check if everything is read (can be removed later)*/
        if(rwbuf[4] == DC_PEC(rwbuf,4))             //Check if PEC matches
        {
            return MAX17841_NO_ERROR;
        }
        else
        {
            for(i=0;i<5;i++)
            {
                pc.printf("%02x\n\r", rwbuf[i]);
            }
            pc.printf("WriteAll PEC Error at Reg %d: Calc=%d Pec Rcv=%d\r\n", rwbuf[1], DC_PEC(rwbuf,6), rwbuf[6]);
            return MAX17841_ERROR;
        }
    }
    else 
    {
        return MAX17841_BUSY;
    }
}

char MAX17841::DC_writeDevice(char u5Device, char reg, unsigned short regval)
{
    rwbuf[0] = 0x05;                     //Length on UART
    rwbuf[1] = (u5Device << 3) + 0x02;   //WRITEDEVICE + DC Address
    rwbuf[2] = (char)reg;                //Reg Address
    rwbuf[3] = (char)(regval & 0x00FF);  //LSB
    rwbuf[4] = (char)(regval >> 8);      //MSB
    rwbuf[5] = DC_PEC(&rwbuf[1],4);      //PEC

    WriteBytes(A_CLR_TX_BUF);
    WriteBytes(A_CLR_RX_BUF);

    WriteBytes(A_WR_LD_Q, rwbuf, 6);            //Transfer data to TX Buffer (+1 Alive CTR)
    WriteBytes(A_WR_NXT_LD_Q);                  //Transmit TX Buffer

    //check if bus is still busy and return fault if it is
    return MAX17841_NO_ERROR;
}

char MAX17841::DC_writeDevice_RB(void)
{
    char tempval, i;

    ReadBytes(R_STATUS_RX, &tempval, 1);        /*Read out RX register for received preambles*/

    if((tempval & 0x12) == 0x12)                //RX Stop Received
    {
        ReadBytes(A_RX_RD_NXT_MSG, rwbuf, 5);   //Read RX Buffer
        ReadBytes(R_STATUS_RX, &tempval, 1);        /*Check if everything is read (can be removed later)*/
        if(rwbuf[4] == DC_PEC(rwbuf,4))            //Check if PEC matches
        {
            return MAX17841_NO_ERROR;
        }
        else
        {
            for(i=0;i<5;i++)
            {
                pc.printf("%02x\n\r", rwbuf[i]);
            }
            pc.printf("WriteDevice PEC Error at Reg %02x: Calc=%02x Pec Rcv=%d02x\r\n", rwbuf[1], DC_PEC(rwbuf,6), rwbuf[5]);
            return MAX17841_ERROR;
        }
    }
    else 
    {
        return MAX17841_BUSY;
    }
}

char MAX17841::DC_readAll(char reg)
{
    rwbuf[0] = 0x04 + (2 * u5DaisyChainSize); //Length depending on DC size
    rwbuf[1] = 0x03;                        //READALL command
    rwbuf[2] = reg;                         //Register to read
    rwbuf[3] = 0x00;                        //DC Byte
    rwbuf[4] = DC_PEC(&rwbuf[1],3);         //PEC

    WriteBytes(A_CLR_TX_BUF);
    WriteBytes(A_CLR_RX_BUF);

    WriteBytes(A_WR_LD_Q, rwbuf, 5);            //Load data in buffer
    WriteBytes(A_WR_NXT_LD_Q);                  //transmit buffer to UART

    //check if bus is still busy and return fault if it is
    return MAX17841_NO_ERROR;
}

char MAX17841::DC_readAll_RB(unsigned short* values)
{
    char tempval, i;

    ReadBytes(R_STATUS_RX, &tempval, 1);        /*Read out RX register for received preambles*/
    
    if((tempval & 0x12) == 0x12)                //RX Stop Received
    {
        ReadBytes(A_RX_RD_NXT_MSG, rwbuf, 0x04 + (2 * u5DaisyChainSize));   //Read RX Buffer
        ReadBytes(R_STATUS_RX, &tempval, 1);        /*Check if everything is read (can be removed later)*/
        if(rwbuf[0x04 + (2 * u5DaisyChainSize) - 1] == DC_PEC(rwbuf,0x04 + (2 * u5DaisyChainSize)-1))            //Check if PEC matches
        {
            for (itr=0;itr<u5DaisyChainSize*2;itr+=2)
            {
                values[itr/2] = rwbuf[itr+3];   //MSB in buffer
                values[itr/2] <<= 8;            //shift MSB by 8
                values[itr/2] += rwbuf[itr+2];    //add LSB
            }
            return MAX17841_NO_ERROR;
        }
        else
        {
            for(i=0;i<(4+(2*u5DaisyChainSize));i++)
            {
                pc.printf("%02x\n\r", rwbuf[i]);
            }
            pc.printf("ReadAll PEC Error at Reg %02x: Calc=%02x Pec Rcv=%02x\r\n", rwbuf[1], DC_PEC(rwbuf,0x04 + (2 * u5DaisyChainSize)-1), rwbuf[0x04 + (2 * u5DaisyChainSize)]);
            return MAX17841_ERROR;
        }
    }
    else 
    {
        return MAX17841_BUSY;
    }
}

