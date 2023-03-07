#ifndef __MAX17851_INTERFACE_H
#define __MAX17851_INTERFACE_H

/*
MAX17851.WConfig S_17851_Conf[10] =   {MAX17851.W_CONFIG_GPIO34,0x22,
                                        MAX17851.W_CONFIG_GPIO12,0x22}; 
*/


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
char MAX17851_readConfig(char MS, RConfig *ConfigRegs, char NumRegs);

/**
    * @brief   Initialize register State
    * @details Write an array of registers and the associate values
    * @param   ConfigRegs All registers and its config
    * @param   NumRegs Size of ConfigRegs
    * @returns 0 if no errors, -1 if error
    */
char MAX17851_writeConfig(char MS, WConfig* ConfigRegs, char NumRegs);

#define VERSION         	(char) 0x00
#define ADDR	         	(char) 0x01
#define STATUS1         	(char) 0x02
#define STATUS2         	(char) 0x03
#define STATUS3         	(char) 0x04
#define FMEA1           	(char) 0x05
#define FMEA2           	(char) 0x06
#define ALRTSUM         	(char) 0x07
#define ALRTOVCELL      	(char) 0x08
#define ALRTUVCELL      	(char) 0x09
#define MINMAXCELL      	(char) 0x0A
#define ALRTAUXPRTCTREG 	(char) 0x0B
#define ALRTAUXOVREG    	(char) 0x0C
#define ALRTAUXUVREG    	(char) 0x0D
#define ALRTCOMPOVREG   	(char) 0x0E
#define ALRTCOMPUVREG   	(char) 0x0F
#define ALRTCOMPAUXOVREG	(char) 0x10
#define ALRTCOMPAUXUVREG	(char) 0x11
#define ALRTBALSWREG    	(char) 0x12
#define SWACTION        	(char) 0x13
#define DEVCFG1         	(char) 0x14
#define DEVCFG2         	(char) 0x15
#define AUXGPIOCFG  		(char) 0x16
#define GPIOCFG     		(char) 0x17
#define PACKCFG     		(char) 0x18
#define ALRTIRQEN   		(char) 0x19
#define ALRTOVEN    		(char) 0x1A
#define ALRTUVEN    		(char) 0x1B
#define ALRTAUXOVEN 		(char) 0x1C
#define ALRTAUXUVEN 		(char) 0x1D
#define ALRTCALTST  		(char) 0x1E
#define OVTHCLRREG  		(char) 0x1F
#define OVTHSETREG  		(char) 0x20
#define UVTHCLRREG  		(char) 0x21
#define UVTHSETREG  		(char) 0x22
#define MSMTCHREG   		(char) 0x23
#define BIPOVTHCLRREG 	    (char) 0x24
#define BIPOVTHSETREG 	    (char) 0x25
#define BIPUVTHCLRREG 	    (char) 0x26
#define BIPUVTHSETREG	    (char) 0x27
#define BLKOVTHCLRREG   	(char) 0x28
#define BLKOVTHSETREG	    (char) 0x29
#define BLKUVTHCLRREG   	(char) 0x2A
#define BLKUVTHSETREG   	(char) 0x2B
#define CSAOVTHCLRREG   	(char) 0x2C
#define CSAOVTHSETREG 	    (char) 0x2D
#define CSAUVTHCLRREG 	    (char) 0x2E
#define CSAUVTHSETREG   	(char) 0x2F
#define AUXROVTHCLRREG      (char) 0x30
#define AUXROVTHSETREG    (char) 0x31
#define AUXRUVTHCLRREG    (char) 0x32
#define AUXRUVTHSETREG    (char) 0x33
#define AUXAOVTHCLRREG    (char) 0x34
#define AUXAOVTHSETREG    (char) 0x35
#define AUXAUVTHCLRREG    (char) 0x36
#define AUXAUVTHSETREG    (char) 0x37
#define COMPOVTHREG       (char) 0x38
#define COMPUVTHREG       (char) 0x39
#define COMPAUXROVTHREG   (char) 0x3A
#define COMPAUXRUVTHREG   (char) 0x3B
#define COMPAUXAOVTHREG   (char) 0x3C
#define COMPAUXAUVTHREG   (char) 0x3D
#define COMPOPNTHREG      (char) 0x3E
#define COMPAUXROPNTHREG  (char) 0x3F
#define COMPAUXAOPNTHREG  (char) 0x40
#define COMPACCOVTHREG    (char) 0x41
#define COMPACCUVTHREG    (char) 0x42
#define BALSHRTTHRREG     (char) 0x43
#define BALLOWTHRREG      (char) 0x44
#define BALHIGHTHRREG     (char) 0x45
#define CSAREG            (char) 0x46
#define CELL1REG          (char) 0x47
#define CELL2REG          (char) 0x48
#define CELL3REG          (char) 0x49
#define CELL4REG          (char) 0x4A
#define CELL5REG          (char) 0x4B
#define CELL6REG          (char) 0x4C
#define CELL7REG          (char) 0x4D
#define CELL8REG          (char) 0x4E
#define CELL9REG          (char) 0x4F
#define CELL10REG         (char) 0x50
#define CELL11REG         (char) 0x51
#define CELL12REG         (char) 0x52
#define CELL13REG         (char) 0x53
#define CELL14REG         (char) 0x54
#define BLOCKREG          (char) 0x55
#define TOTALREG          (char) 0x56
#define DIAG1REG          (char) 0x57
#define DIAG2REG          (char) 0x58
#define AUX0REG           (char) 0x59
#define AUX1REG           (char) 0x5A
#define AUX2REG           (char) 0x5B
#define AUX3REG           (char) 0x5C
#define AUX4REG           (char) 0x5D
#define AUX5REG           (char) 0x5E
#define POLARITYCTRL      (char) 0x5F
#define AUXREFCTRL        (char) 0x60
#define AUXTIMEREG        (char) 0x61
#define ACQCFG            (char) 0x62
#define BALSWDLY          (char) 0x63
#define MEASUREEN1        (char) 0x64
#define MEASUREEN2        (char) 0x65
#define SCANCTRL          (char) 0x66
#define ADCTEST1AREG      (char) 0x67
#define ADCTEST1BREG      (char) 0x68
#define ADCTEST2AREG      (char) 0x69
#define ADCTEST2BREG      (char) 0x6A
#define DIAGCFG           (char) 0x6B
#define CTSTCFG           (char) 0x6C
#define AUXTSTCFG         (char) 0x6D
#define DIAGGENCFG        (char) 0x6E
#define BALSWCTRL         (char) 0x6F
#define BALEXP1           (char) 0x70
#define BALEXP2           (char) 0x71
#define BALEXP3           (char) 0x72
#define BALEXP4           (char) 0x73
#define BALEXP5           (char) 0x74
#define BALEXP6           (char) 0x75
#define BALEXP7           (char) 0x76
#define BALEXP8           (char) 0x77
#define BALEXP9           (char) 0x78
#define BALEXP10          (char) 0x79
#define BALEXP11          (char) 0x7A
#define BALEXP12          (char) 0x7B
#define BALEXP13          (char) 0x7C
#define BALEXP14          (char) 0x7D
#define BALAUTOUVTHR      (char) 0x7E
#define BALDLYCTRL        (char) 0x7F
#define BALCTRL           (char) 0x80
#define BALSTAT           (char) 0x81
#define BALUVSTAT         (char) 0x82
#define BALDATA           (char) 0x83
#define I2CPNTR           (char) 0x84
#define I2CWDATA          (char) 0x85
#define I2CWDATA2         (char) 0x86
#define I2CRDATA          (char) 0x87
#define I2CRDATA2         (char) 0x88
#define I2CCFG            (char) 0x89
#define I2CSTAT           (char) 0x8A
#define I2CSEND           (char) 0x8B
#define ID1               (char) 0x8C
#define ID2               (char) 0x8D
#define OTP2              (char) 0x8E
#define OTP3              (char) 0x8F
#define OTP4              (char) 0x90
#define OTP5              (char) 0x91
#define OTP6              (char) 0x92
#define OTP7              (char) 0x93
#define OTP8              (char) 0x94
#define OTP0              (char) 0x95
#define OTP10             (char) 0x96
#define OTP11             (char) 0x97
#define OTP12             (char) 0x98

/**
    * @brief   Initialize BMS bridge system
    * @details Defining settings, GPIO and waking up BMS system
    * @param ulWakeuptime Pointer to waikuptime in us
    * @param u5DaisySize Pointer to be filled with the actual chain size
    * @returns 0 if no errors, -1 if error
    */
char MAX17851_Init(unsigned long *ulWakeuptime, char *u5DaisySize);

/**
    * @brief   Resets the specific IC
    * @details Executes SWPOR Command and waits 10us
    * @returns 0 if no errors, -1 if error
    */
char MAX17851_Reset(char MS);

/**
    * @brief   Sets GPIO of MAX17851 (GPIO1&2 Only)
    * @details For OV UV detection
    * @returns 0 if no errors, -1 if error
    */
char MAX17851_SetGPIO(char MS, char GPIO12);

/**
    * @brief   Initiates a write register function to daisy chain devices
    * @details Register value will be written to all devices in the chain
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param reg register to be written (MAX17852Register_t)
    * @param regval register value (u16)
    * @param blockingTimeout_us u16 timeout in microseconds <b>after Transmission has initiated</b>, meaning UART timeout only. 0 = non-blocking, otherwise waits for result within function and analyzes PEC. When non-blocking, it's recommended to use @ref MAX17852_writeAll_RB function
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_writeAll(char MS, char reg, short regval, unsigned int blockingTimeout_us);

/**
    * @brief   Checks if the write has been successful
    * @details Checks the outgoing message against the received one
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_writeAll_RB(char MS);

/**
    * @brief   Initiates a write register function to ONE daisy chain device
    * @details Only the selected address is affected
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param u5Device Device address within the daisy chain (same as READBLOCK)
    * @param reg register to be written (MAX17852Register_t)
    * @param regval register value (u16)
    * @param blockingTimeout_us u16 timeout in microseconds <b>after Transmission has initiated</b>, meaning UART timeout only. 0 = non-blocking, otherwise waits for result within function and analyzes PEC. When non-blocking, it's recommended to use @ref MAX17852_writeAll_RB function
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_writeDevice(char MS, char u5Device, char reg, short regval, unsigned int blockingTimeout_us);

/**
    * @brief   Checks if the write has been successful
    * @details Checks the outgoing message against the received one
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_writeDevice_RB(char MS);

/**
    * @brief   Initiates a single register of all devices in the daisy chain
    * @details This function loads the TX Queue and sends it via the Daisy Chain
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param reg register to be written (MAX17852Register_t)
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_readAll(char MS, char reg);

/**
    * @brief   Initiates a single register of all devices in the daisy chain
    * @details Lowest device is on lowest index
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param values Address of result cluster
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_readAll_RB(char MS, short* values);

/**
    * @brief   Initiates a block transfer of registers from a single device
    * @details This function loads the TX Queue and sends it via the Daisy Chain
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param u5Device 5 bit device address in the daisy chain
    * @param u8StartAddr Start Address of the block transfer
    * @param u5BlockSize 5 bit block size (max. 31 registers)
    * @param reg register to be written (MAX17852Register_t)
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_readBlock(char MS, char u5Device, char u8StartAddr, char u5BlockSize);

/**
    * @brief   Transfers a previously finished block transfer into MCU memory
    * @details Lowest address is on lowest index
    * @param MS 'M' for Master interface, 'S' for Slave interface of MAX17851
    * @param values Address of result cluster
    * @returns 0 if no errors, -1 if error
    */
char MAX17852_readBlock_RB(char MS, short* values);

/**
* @brief   Cyclic MAX17851 task
* @details Checks functionality/safety and pets watchdog
*/
void MAX17851_Loop(void);

#endif
