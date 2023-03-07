#ifndef __SENDREPORT_H
#define __SENDREPORT_H

typedef struct{
    int csavalue;
    int cellvalues[14];
    int blockvalue;
    int totalvalue;
    int diag[12];
    int auxvalues[6];
    int statusvalues[19];
    int BalancingSwitches;
    int BalancingUVStatus;
    int externalAux[16];
    int SM59[3];    //SM59[0] = ALRTSUM, SM59[1] = CompOVTH, SM59[2] = CompUVTH
}TransmitData;

typedef enum
{
    IDLE,
    BUSY,
    DONE,
    ERR
}SendReportState;

/**
    * @brief   Copies data to the transmit buffer and transfers it to the host
    * @details copies data to local memory structure and sends it out with every Transfer function call until finished
    * @param DeviceData pointer to the cell measurrement Data array
    * @returns IDLE, BUSY, DONE
    */
SendReportState Report_InitTransfer(DeviceData *DataToSend, int NumArrays);

/**
    * @brief   Processes the transfer until all data is transmitted
    * @details Sends as many characters as possible within one tick
    * @returns IDLE, BUSY, DONE
    */
SendReportState Report_Process(void);

/**
    * @brief   Returns state of the process
    * @details No action is taken
    * @returns IDLE, BUSY, DONE
    */
SendReportState GetState(void);

#endif
