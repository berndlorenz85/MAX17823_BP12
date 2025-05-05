#ifndef __Scheduler_H_
#define __Scheduler_H_

//SCHEDULER
#define LOOPTICKS_MS              100

#define SCAN1                     0
#define READBACK0_CELL_TICK1      SCAN1+1   //Start Analog Readback
#define READBACK0_CELL_TICK2      SCAN1+3   //Finish Analog Readback
#define READBACK0_STATUS_TICK1    SCAN1+5   //Start STATUS Readback
#define READBACK0_STATUS_TICK2    SCAN1+7   //Finish STATUS Readback
#define SM59_CHECK_RESULTS        SCAN1+10  //Checking ALERTS and reset limits
#define PRINT1_0                  SCAN1+15  //Printing Analog Readings because we have time
#define PRINT1_1                  SCAN1+16
#define PRINT1_2                  SCAN1+17
#define PRINT1_3                  SCAN1+18
#define PRINT1_4                  SCAN1+19
#define PRINT1_5                  SCAN1+20  //Printing Status Values
#define PRINT1_6                  SCAN1+21
#define PRINT1_7                  SCAN1+22
#define PRINT1_8                  SCAN1+23
#define PRINT1_9                  SCAN1+24

#define SCAN2                     25
#define READBACK1_CELL_TICK1      SCAN2+1   //Start Analog Readback
#define READBACK1_CELL_TICK2      SCAN2+3   //Finish Analog Readback
#define READBACK1_STATUS_TICK1    SCAN2+5   //Start STATUS Readback
#define READBACK1_STATUS_TICK2    SCAN2+7  //Finish STATUS Readback
#define CHECK_OVUVALERTS1         SCAN2+10  //Displaying any Status Alerts
#define PRINT2_0                  SCAN2+15
#define PRINT2_1                  SCAN2+16
#define PRINT2_2                  SCAN2+17
#define PRINT2_3                  SCAN2+18
#define PRINT2_4                  SCAN2+19
#define PRINT2_5                  SCAN2+20
#define PRINT2_6                  SCAN2+21
#define PRINT2_7                  SCAN2+22
#define PRINT2_8                  SCAN2+23
#define PRINT2_9                  SCAN2+24

#define SCAN3                     50
#define READBACK2_CELL_TICK1      SCAN3+1   //Start Analog Readback
#define READBACK2_CELL_TICK2      SCAN3+3   //Finish Analog Readback
#define READBACK2_STATUS_TICK1    SCAN3+5   //Start STATUS Readback
#define READBACK2_STATUS_TICK2    SCAN3+7  //Finish STATUS Readback
#define BALANCING_TICK1           SCAN3+10  //Determining OV and UV
#define BALANCING_TICK2           SCAN3+12  //Enabling the determined cell and display LED (measurement needs to be finished)
#define PRINT3_0                  SCAN3+14
#define PRINT3_1                  SCAN3+15
#define PRINT3_2                  SCAN3+16
#define PRINT3_3                  SCAN3+17
#define PRINT3_4                  SCAN3+18
#define PRINT3_5                  SCAN3+19
#define PRINT3_6                  SCAN3+20
#define PRINT3_7                  SCAN3+21
#define PRINT3_8                  SCAN3+22
#define PRINT3_9                  SCAN3+23


#define SM59_PREP                 75              //For SM59 Evaluation
#define SCAN_ALTMUX               SM59_PREP+1     //Can be evaluated after STATUS
#define READBACK3_CELL_TICK1      SM59_PREP+2     //Start Analog Readback
#define READBACK3_CELL_TICK2      SM59_PREP+4     //Finish Analog Readback
#define READBACK3_STATUS_TICK1    SM59_PREP+6     //Start STATUS Readback
#define READBACK3_STATUS_TICK2    SM59_PREP+8     //Finish STATUS Readback
#define PRINT4_0                  SM59_PREP+15
#define PRINT4_1                  SM59_PREP+16
#define PRINT4_2                  SM59_PREP+17
#define PRINT4_3                  SM59_PREP+18
#define PRINT4_4                  SM59_PREP+19
#define PRINT4_5                  SM59_PREP+20
#define PRINT4_6                  SM59_PREP+21
#define PRINT4_7                  SM59_PREP+22
#define PRINT4_8                  SM59_PREP+23
#define PRINT4_9                  SM59_PREP+24

char Scheduler_Init(void);

void Scheduler_Loop(void);


#endif
