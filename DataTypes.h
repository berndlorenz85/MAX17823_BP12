#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

//Defining readback structure
typedef struct{
    short csavalue;
    short cellvalues[14];
    short blockvalue;
    short totalvalue;
    short diag1;
    short diag2;
    short auxvalues[6];
    short statusvalues[19];
    short Balancing[3];   //BALSW, BALSTAT, BALUVSTAT
    short SM59[3];    //ALERTSUM, COMPOV, COMPUV
    //add variables to read here
}DeviceData;

typedef struct{
    unsigned int mAH_Counter;
    unsigned char Percentage;
    unsigned short BattVoltage[16];
    unsigned short BusBarVoltage[12];
    
}SendData;

#endif
