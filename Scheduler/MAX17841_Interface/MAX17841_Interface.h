#ifndef __MAX17841_INTERFACE_H
#define __MAX17841_INTERFACE_H


/**
* @brief   Write Config Key-Value
* @details Contains a map of key value pairs for IC configuration
*/
typedef struct wConfig{
    char WriteReg;
    char RegValue;
}WConfig;

/**
* @brief   Read Config Key-Value
* @details Contains a map of key value pairs for IC configuration
*/
typedef struct rConfig{
    char ReadReg;
    char RegValue;
}RConfig;

/**
    * @brief   Initialize register State
    * @details Read an array of registers and the associate values
    * @param   ConfigRegs Pointer to read config register
    * @param   NumRegs Number of bytes to be read
    * @returns 0 if no errors, -1 if error
    */
char MAX17841_readConfig(char MS, RConfig *ConfigRegs, char NumRegs);

/**
    * @brief   Initialize register State
    * @details Write an array of registers and the associate values
    * @param   ConfigRegs All registers and its config
    * @param   NumRegs Size of ConfigRegs
    * @returns 0 if no errors, -1 if error
    */
char MAX17841_writeConfig(char MS, WConfig* ConfigRegs, char NumRegs);

#define VERSION         	(char) 0x00
#define ADDR	         	(char) 0x01
#define STATUS         	    (char) 0x02
#define FMEA1         	    (char) 0x03
#define ALRTCELL         	(char) 0x04
#define ALRTOVCELL         	(char) 0x05
#define ALRTUVCELL         	(char) 0x07
#define ALRTBALSW         	(char) 0x08
#define MINMAXCELL      	(char) 0x0A
#define FMEA2            	(char) 0x0B
#define ID1    	            (char) 0x0D
#define ID2             	(char) 0x0E
#define DEVCFG1         	(char) 0x10
#define GPIO            	(char) 0x11
#define MEASUREEN         	(char) 0x12
#define SCANCTRL        	(char) 0x13
#define ALRTOVEN         	(char) 0x14
#define ALRTUVEN         	(char) 0x15
#define TIMERCFG      		(char) 0x18
#define ACQCFG       		(char) 0x19
#define BALSWEN     		(char) 0x1A
#define DEVCFG2     		(char) 0x1B
#define BALDIAGCFG   		(char) 0x1C
#define BALSWDCHG    		(char) 0x1D
#define TOPCELL       		(char) 0x1E

#define CELL1  		        (char) 0x20
#define CELL2  		        (char) 0x21
#define CELL3  		        (char) 0x22
#define CELL4   		    (char) 0x23
#define CELL5 	            (char) 0x24
#define CELL6 	            (char) 0x25
#define CELL7 	            (char) 0x26
#define CELL8	            (char) 0x27
#define CELL9   	        (char) 0x28
#define CELL10	            (char) 0x29
#define CELL11   	        (char) 0x2A
#define CELL12   	        (char) 0x2B
#define BLOCK              	(char) 0x2C
#define AIN1         	    (char) 0x2D
#define AIN2         	    (char) 0x2E
#define TOTAL              	(char) 0x2F

#define OVTHCLR             (char) 0x40
#define OVTHSET             (char) 0x42
#define UVTHCLR             (char) 0x44
#define UVTHSET             (char) 0x46
#define MSMTCH              (char) 0x48
#define AINOT               (char) 0x49
#define AINUT               (char) 0x4A
#define BALSHRTTHR          (char) 0x4B
#define BALLOWTHR           (char) 0x4C
#define BALHIGHTHR          (char) 0x4D
#define DIAG                (char) 0x50
#define DIAGCFG             (char) 0x51
#define CTSTCFG             (char) 0x52

#define ADCTEST1A          (char) 0x57
#define ADCTEST1B          (char) 0x58
#define ADCTEST2A          (char) 0x59
#define ADCTEST2B          (char) 0x5A

/**
    * @brief   Initialize BMS bridge system
    * @details Defining settings, GPIO and waking up BMS system
    * @param ulWakeuptime Pointer to waikuptime in us
    * @param u5DaisySize Pointer to be filled with the actual chain size
    * @returns 0 if no errors, -1 if error
    */
char MAX17841_Init(char *u5DaisySize);

/**
    * @brief   Initiates a write register function to daisy chain devices
    * @details Register value will be written to all devices in the chain
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @param reg register to be written (MAX17823Register_t)
    * @param regval register value (u16)
    * @param blockingTimeout_us u16 timeout in microseconds <b>after Transmission has initiated</b>, meaning UART timeout only. 0 = non-blocking, otherwise waits for result within function and analyzes PEC. When non-blocking, it's recommended to use @ref MAX17823_writeAll_RB function
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_writeAll(char MS, char reg, unsigned short regval, unsigned int blockingTimeout_us);

/**
    * @brief   Checks if the write has been successful
    * @details Checks the outgoing message against the received one
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_writeAll_RB(char MS);

/**
    * @brief   Initiates a write register function to ONE daisy chain device
    * @details Only the selected address is affected
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @param u5Device Device address within the daisy chain (same as READBLOCK)
    * @param reg register to be written (MAX17823Register_t)
    * @param regval register value (u16)
    * @param blockingTimeout_us u16 timeout in microseconds <b>after Transmission has initiated</b>, meaning UART timeout only. 0 = non-blocking, otherwise waits for result within function and analyzes PEC. When non-blocking, it's recommended to use @ref MAX17823_writeAll_RB function
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_writeDevice(char MS, char u5Device, char reg, unsigned short regval, unsigned int blockingTimeout_us);

/**
    * @brief   Checks if the write has been successful
    * @details Checks the outgoing message against the received one
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_writeDevice_RB(char MS);

/**
    * @brief   Initiates a single register of all devices in the daisy chain
    * @details This function loads the TX Queue and sends it via the Daisy Chain
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @param reg register to be written (MAX17823Register_t)
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_readAll(char MS, char reg);

/**
    * @brief   Initiates a single register of all devices in the daisy chain
    * @details Lowest device is on lowest index
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17841
    * @param values Address of result cluster
    * @returns 0 if no errors, -1 if error
    */
char MAX17823_readAll_RB(char MS, unsigned short* values);

#endif
