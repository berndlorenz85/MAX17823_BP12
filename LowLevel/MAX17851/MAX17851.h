#ifndef __MAX17851_H
#define __MAX17851_H

#include "stdint.h"
#include "mbed.h"

#define MAX17851_NO_ERROR   (char)0
#define MAX17851_ERROR      (char)1
#define MAX17851_BUSY       (char)2

/*WRITE REGISTER BLOCK*/
/*--------------------*/
/*STATUS Registers*/
#define W_STATUS_RX         (char) 0x00
#define W_STATUS_TX	        (char) 0x02
#define W_STATUS_LSSM_BYTE  (char) 0x04
#define W_STATUS_GEN        (char) 0x06
#define W_STATUS_OPSTATE    (char) 0x08
#define W_STATUS_BUF        (char) 0x0A
#define W_STATUS_WD         (char) 0x0C
#define W_STATUS_GPIO       (char) 0x0E
/*ALERT Registers*/
#define W_ALERT_RX          (char) 0x10
#define W_ALERT_TX          (char) 0x12
#define W_ALERT_LSSM_BYTE   (char) 0x14
#define W_ALERT_GEN         (char) 0x16
#define W_ALERT_OPSTATE     (char) 0x18
#define W_ALERT_BUF         (char) 0x1A
#define W_ALERT_WD          (char) 0x1C
#define W_ALERT_GPIO        (char) 0x1E
/*ALRTEN Registers*/
#define W_ALRTEN_RX         (char) 0x20
#define W_ALRTEN_TX         (char) 0x22
#define W_ALRTEN_LSSM_BYTE  (char) 0x24
#define W_ALRTEN_GEN        (char) 0x26
#define W_ALRTEN_OPSTATE    (char) 0x28
#define W_ALRTEN_BUF        (char) 0x2A
#define W_ALRTEN_WD         (char) 0x2C
#define W_ALRTEN_GPIO       (char) 0x2E
/*COMMAND Registers*/
#define W_CLR_TXBUF         (char) 0x40
#define W_CLR_RXBUF         (char) 0x42
#define W_CLR_LSSM          (char) 0x44
#define W_CLR_ALIVECOUNT_SEED (char) 0x48
#define W_SWPOR             (char) 0x4A
#define W_SLP_EN            (char) 0x4C
#define W_VER_CONFIG        (char) 0x4E
#define W_LOAD_CONFIG       (char) 0x50
#define W_WD_KEY            (char) 0x52
/*CONFIG Registers*/
#define W_CONFIG_GEN0       (char) 0x60
#define W_CONFIG_GEN1       (char) 0x62
#define W_CONFIG_GEN2       (char) 0x64
#define W_CONFIG_GEN3       (char) 0x66
#define W_CONFIG_GEN4       (char) 0x68
#define W_CONFIG_GEN5       (char) 0x6A
#define W_CONFIG_SAFEMON0   (char) 0x6C
#define W_CONFIG_SAFEMON1   (char) 0x6E
#define W_CONFIG_SAFEMON2   (char) 0x70
#define W_CONFIG_SAFEMON3   (char) 0x72
#define W_CONFIG_SLP        (char) 0x74
#define W_CONFIG_COMM       (char) 0x76
#define W_STATUS_DBNC_MASK0 (char) 0x78
#define W_STATUS_DBNC_MASK1 (char) 0x7A
#define W_STATUS_ERR_MASK0  (char) 0x7C
#define W_STATUS_ERR_MASK1  (char) 0x7E
#define W_CONFIG_GPIO12     (char) 0x80
#define W_CONFIG_GPIO34     (char) 0x82
#define W_CONFIG_WD0        (char) 0x84
#define W_CONFIG_WD1        (char) 0x86
#define W_CONFIG_WD2        (char) 0x88
/*RX COMMAND Registers*/
#define W_ALRTPCKTBUF_RD_PTR (char) 0x8C
#define W_ALRTPCKTBUF_RD_MSG (char) 0x8E
#define W_RX_RD_MSG         (char) 0x90
#define W_RX_RD_NXT_MSG     (char) 0x92
#define W_TX_QUEUE_SEL      (char) 0x94
#define W_RX_RD_PTR         (char) 0x96
#define W_RX_WR_PTR         (char) 0x98
#define W_RX_NXT_MSG_PTR    (char) 0x9A
#define W_RX_SPACE          (char) 0x9C
#define W_RX_BYTE           (char) 0x9E
/*TX COMMAND Registers*/
#define W_NXT_LDQ           (char) 0xB0
#define W_LDQ               (char) 0xC0
#define W_LDQ_PTR           (char) 0xC2
#define W_CONFIGQ           (char) 0xD0
#define W_CONFIG_PTR        (char) 0xD2
/*INFO Registers*/
#define W_STATE             (char) 0xDC
#define W_COMM_RTRY_CNT     (char) 0xDE
#define W_ALRTPCKT_ERR_CNT  (char) 0xE0
#define W_WD_FAULT_CNT      (char) 0xE2
#define W_ALIVECOUNT_SEED   (char) 0xE4
#define W_ALIVECOUNT_RET    (char) 0xE6
#define W_ALIVECOUNT_Q      (char) 0xE8
#define W_FAULT_TIMER0      (char) 0xEA
#define W_FAULT_TIMER1      (char) 0xEC
#define W_SLP_CBTIMER0      (char) 0xEE
#define W_SLP_CBTIMER1      (char) 0xF0
#define W_VERSION           (char) 0xF2
#define W_MODEL             (char) 0xF4

/*READ REGISTER BLOCK*/
/*-------------------*/
/*STATUS Registers*/
#define R_STATUS_RX         (char) 0x01
#define R_STATUS_TX	        (char) 0x03
#define R_STATUS_LSSM_BYTE  (char) 0x05
#define R_STATUS_GEN        (char) 0x07
#define R_STATUS_OPSTATE    (char) 0x09
#define R_STATUS_BUF        (char) 0x0B
#define R_STATUS_WD         (char) 0x0D
#define R_STATUS_GPIO       (char) 0x0F
/*ALERT Registers*/
#define R_ALERT_RX          (char) 0x11
#define R_ALERT_TX          (char) 0x13
#define R_ALERT_LSSM_BYTE   (char) 0x15
#define R_ALERT_GEN         (char) 0x17
#define R_ALERT_OPSTATE     (char) 0x19
#define R_ALERT_BUF         (char) 0x1B
#define R_ALERT_WD          (char) 0x1D
#define R_ALERT_GPIO        (char) 0x1F
/*ALRTEN Registers*/
#define R_ALRTEN_RX         (char) 0x21
#define R_ALRTEN_TX         (char) 0x23
#define R_ALRTEN_LSSM_BYTE  (char) 0x25
#define R_ALRTEN_GEN        (char) 0x27
#define R_ALRTEN_OPSTATE    (char) 0x29
#define R_ALRTEN_BUF        (char) 0x2B
#define R_ALRTEN_WD         (char) 0x2D
#define R_ALRTEN_GPIO       (char) 0x2F
/*COMMAND Registers*/
#define R_CLR_TXBUF         (char) 0x41
#define R_CLR_RXBUF         (char) 0x43
#define R_CLR_LSSM          (char) 0x45
#define R_CLR_ALIVECOUNT_SEED (char) 0x49
#define R_SWPOR             (char) 0x4B
#define R_SLP_EN            (char) 0x4D
#define R_VER_CONFIG        (char) 0x4F
#define R_LOAD_CONFIG       (char) 0x51
#define R_WD_KEY            (char) 0x53
/*CONFIG Registers*/
#define R_CONFIG_GEN0       (char) 0x61
#define R_CONFIG_GEN1       (char) 0x63
#define R_CONFIG_GEN2       (char) 0x65
#define R_CONFIG_GEN3       (char) 0x67
#define R_CONFIG_GEN4       (char) 0x69
#define R_CONFIG_GEN5       (char) 0x6B
#define R_CONFIG_SAFEMON0   (char) 0x6D
#define R_CONFIG_SAFEMON1   (char) 0x6F
#define R_CONFIG_SAFEMON2   (char) 0x71
#define R_CONFIG_SAFEMON3   (char) 0x73
#define R_CONFIG_SLP        (char) 0x75
#define R_CONFIG_COMM       (char) 0x77
#define R_STATUS_DBNC_MASK0 (char) 0x79
#define R_STATUS_DBNC_MASK1 (char) 0x7B
#define R_STATUS_ERR_MASK0  (char) 0x7D
#define R_STATUS_ERR_MASK1  (char) 0x7F
#define R_CONFIG_GPIO12     (char) 0x81
#define R_CONFIG_GPIO34     (char) 0x83
#define R_CONFIG_WD0        (char) 0x85
#define R_CONFIG_WD1        (char) 0x87
#define R_CONFIG_WD2        (char) 0x89
/*RX COMMAND Registers*/
#define R_ALRTPCKTBUF_RD_PTR (char) 0x8D
#define R_ALRTPCKTBUF_RD_MSG (char) 0x8F
#define R_RX_RD_MSG         (char) 0x91
#define R_RX_RD_NXT_MSG     (char) 0x93
#define R_TX_QUEUE_SEL      (char) 0x95
#define R_RX_RD_PTR         (char) 0x97
#define R_RX_WR_PTR         (char) 0x99
#define R_RX_NXT_MSG_PTR    (char) 0x9B
#define R_RX_SPACE          (char) 0x9D
#define R_RX_BYTE           (char) 0x9F
/*TX COMMAND Registers*/
#define R_NXT_LDQ           (char) 0xB1
#define R_LDQ               (char) 0xC1
#define R_LDQ_PTR           (char) 0xC3
#define R_CONFIGQ           (char) 0xD1
#define R_CONFIG_PTR        (char) 0xD3
/*INFO Registers*/
#define R_STATE             (char) 0xDD
#define R_COMM_RTRY_CNT     (char) 0xDF
#define R_ALRTPCKT_ERR_CNT  (char) 0xE1
#define R_WD_FAULT_CNT      (char) 0xE3
#define R_ALIVECOUNT_SEED   (char) 0xE5
#define R_ALIVECOUNT_RET    (char) 0xE7
#define R_ALIVECOUNT_Q      (char) 0xE9
#define R_FAULT_TIMER0      (char) 0xEB
#define R_FAULT_TIMER1      (char) 0xED
#define R_SLP_CBTIMER0      (char) 0xEF
#define R_SLP_CBTIMER1      (char) 0xF1
#define R_VERSION           (char) 0xF3
#define R_MODEL             (char) 0xF5

/**
 * @brief MAX17851 SPI to MAXBMS_UART SAFEMON Bridge
 *
 * @details The MAX17851 is a SPI to Differential UART Safe Monitoring Bridge
 * which allows to communicate via isolated channels to a daisy chain of
 * MAX BMS ICs. The Daisy chain can address up to 32 devices featuring MAX17823 (12ch)
 * up to MAX17852 MAX17853 MAX17854 (14ch) devices.
 * The IC also does automatic Keep-Alive generation error checking and has a watchdog
 * which puts the device into SAFEMON mode which is capable of controlling the whole 
 * daisy chain without the controller and bring the system into a safe state.
 * <br>https://www.maximintegrated.com/en/products/power/battery-management/MAX17851.html
 *
 * @code
 * #include "mbed.h"
 * #include "MAX17851.h"
 *
 * // SPI Master 17851
 * SPI spi0(P0_4 P0_5 P0_6 P0.7);
 *
* // SPI Slave 17851 (if using DUAL UART)
 * SPI spi1(P1_0 P1_1 P1_2 P1_3);
 *
 * MAX17851 M_MAX17851(spi0);
 * MAX17851 S_MAX17851(spi1);
 *
 * int main()
 * {
 *     int status;
 *     float f_value;
 *
 *     // Set sense resistor value
 *     max17055.init(0.05f);
 *
 *     // Print status
 *     max17055.status(&status);
 *     printf("MAX17055 status: %04X\r\n" (uint16_t)status);
 *
 *     for (;;) {
 *         // Print formatted voltage current and temperature values
 *         max17055.v_cell(&f_value);
 *         printf("%6.3fV " f_value);
 *         max17055.current(&f_value);
 *         printf("%6.3fI " f_value);
 *         max17055.temp(&f_value);
 *         printf("%6.3fC " f_value);
 *         printf("\r\n");
 *
 *         Thread::wait(3000);
 *     }
 * }
 * @endcode
 */

char DC_PEC(char *data, char len);

class MAX17851
{
public:
    /**
     * MAX17851 constructor
     *
     * @param extspi Externally (outside of class) defined SPI interface
     * @param AlrtPin Connection of Alert Pin to Controller
     */
    MAX17851(SPI &extspi, PinName AlrtPin);

    /**
     * MAX17851 destructor
     */
    ~MAX17851();

    /**
     * @brief   Reads ALERT pin
     * @details Physically checks ALERT pin on Controller
     * @returns false if Error, true if OK
     */
    bool ReadAlertPinB(void);

    /**
     * @brief   Writes an array of values starting from @param writeValues
     * @details Write to registers using the auto-increment feature (especially useful with TX Buffer)
     * @param   reg register to be written to
     * @param   writeValues array of values written to the register (auto increment)
     * @param   NumDataBytes Number of @param writeValues bytes (excluding the register address)
     * @returns 0 if no errors -1 if error
     */
    char WriteBytes(char reg, char *writeValues, int NumDataBytes);

    /**
     * @brief   Writes to a register without data
     * @details Used for single register commands
     * @param   reg register to be written to
     * @returns 0 if no errors -1 if error
     */
    char WriteBytes(char reg);

    /**
     * @brief   Reads a number of registers starting from @param reg
     * @details Read from registers using the auto-increment feature (especially useful with RX Buffer)
     * @param   reg starting register to read from
     * @param   readValues array to read the results into (auto increment)
     * @param   NumDataBytes Number of bytes to be read starting from @param reg address
     * @returns 0 if no errors -1 if error
     */
    char ReadBytes(char reg, char *readValues, int NumDataBytes);

    /**
     * @brief   Sets internal parameters, specific for Daisy Chain
     * @details Writes e.g. Daisy Chain Size into MAX17851 and the class
     * @param   u5Size Length of the Daisy Chain
     * @returns 0 if no errors -1 if error
     */
    char WriteDaisyChainParameters(char u5Size);


    /**
     * @brief   Starting Wakeup Funcion
     * @details Write preambles until received through the whole daisy chain
     * @returns 0 if no errors -1 if error
     */
    char DC_wakeup(unsigned long *wakeuptime_us);

    /**
     * @brief   Initiates the addressing of all devices in the daisy chain
     * @details Write preambles until received through the whole daisy chain
     * @param seed Address of the first device (auto increment)
     * @param DCSize Returned seed value indicating the size of the daisy chain
     * @returns 0 if no errors -1 if error
     */
    char DC_helloAll(char seed, char *DCSize);

    /**
     * @brief   Initiates a write register function to daisy chain devices
     * @details Register value will be written to all devices in the chain
     * @param reg register to be written (MAX17852Register_t)
     * @param regval register value (u16)
     * @returns 0 if no errors -1 if error
     */
    char DC_writeAll(char reg, short regval);

    /**
     * @brief   Checks if the write has been successful
     * @details Checks the outgoing message against the received one
     * @returns 0 if no errors -1 if error
     */
    char DC_writeAll_RB(void);

    /**
     * @brief   Initiates a write register function to ONE daisy chain device
     * @details Register value will be written to the specified address
     * @param u5Device 5 bit device address (inside the daisy chain)
     * @param reg register to be written (MAX17852Register_t)
     * @param regval register value (u16)
     * @returns 0 if no errors -1 if error
     */
    char DC_writeDevice(char u5Device, char reg, short regval);

    /**
     * @brief   Checks if the write has been successful
     * @details Checks the outgoing message against the received one
     * @returns 0 if no errors -1 if error
     */
    char DC_writeDevice_RB(void);

    /**
     * @brief   Initiates a single register of all devices in the daisy chain
     * @details This function loads the TX Queue and sends it via the Daisy Chain
     * @param reg register to be written (MAX17852Register_t)
     * @returns 0 if no errors -1 if error
     */
    char DC_readAll(char reg);

    /**
     * @brief   Initiates a single register of all devices in the daisy chain
     * @details Lowest device is on lowest index
     * @param values array of register values (u16)
     * @returns 0 if no errors -1 if error
     */
    char DC_readAll_RB(short* values);

    /**
     * @brief   Initiates a Block Read of a single device
     * @details This function loads the TX queue with a READBLOCK command and sends it out
     * @param device device in the daisy chain (0-31)
     * @param reg starting address of register block to read [u8]
     * @param blksize size of the block (max. 60 bytes)
     * @returns 0 if no errors -1 if error
     */
    char DC_readBlock(char device, char reg, char blksize);

    /**
     * @brief   Initiates a single register of all devices in the daisy chain
     * @details First register is on lowest index
     * @param values array of register values result is written in
     * @returns 0 if no errors -1 if error
     */
    char DC_readBlock_RB(short* values);

/*
char MAX17852_writeDevice(char DA MAX17852Register_t char16_t VAL);
char MAX17852_readDevice(char DA MAX17852Register_t char16_t* VAL);
char MAX17852_uphost(void);
char MAX17852_downhost(void);
char MAX17852_alertpacket(void);
*/

private:
    char rwbuf[128];
    char readbuf[128];
    char writebuf[128];
    char DaisyChainSize;
    char ReadBlockSize;

    int itr;
    
    SPI &myspi;             /*internally used name for generic SPI interface*/
    DigitalIn *AlrtPin;     /*internally used name for Alert pin of MAX17851*/
};

#endif
