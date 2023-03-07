#include "mbed.h"
#include "DataTypes.h"
#include "SendReport.h"

/*
extern Serial pc;

static SendReportState LocState = IDLE;   //State Variable

DeviceData SendData[8];         //local memory for printing
int PrintSizeBytes;              //We only send number of data we need

short* p16SourceAddress;
short* p16DestAddress;

static int i32Print;                   //local print pointer
static char* p8PrintAddress;

SendReportState Report_InitTransfer(DeviceData *DataToSend, int NumArrays)
{
    int i;

    PrintSizeBytes = NumArrays * sizeof(DeviceData);
    p16SourceAddress = (short*)&DataToSend[0];
    p16DestAddress = (short*)&SendData[0];
    for(i=0;i<(PrintSizeBytes/2);i++)
    {
        *(p16DestAddress+i) = *(p16SourceAddress+i);
    }

    p8PrintAddress = (char*)&SendData[0]; //Set starting pointer (currently only 1 device supported)
    i32Print = 0;
    LocState = BUSY;
    return BUSY;
}

SendReportState Report_Process(void)
{
    int i;

    printf("P: %d ", i32Print);
    printf("PSB: %d ", PrintSizeBytes);
    printf("LocState: %d ", LocState);
    if(LocState == BUSY)
    {
        for(i=i32Print;i<PrintSizeBytes;i++)
        {
            pc.putc(*(p8PrintAddress+i));
            i32Print++;
            if(i32Print%20 == 0) break;   //break after 20 bytes
        }
        if(i32Print >= PrintSizeBytes)
        {
            LocState = DONE;
        }
    }
    else {};
    return LocState;
}

SendReportState GetState(void)
{
    return LocState;
}
*/