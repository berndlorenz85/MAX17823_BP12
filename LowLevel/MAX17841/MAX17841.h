#ifndef __MAX17841_H
#define __MAX17841_H

#include "stdint.h"
#include "mbed.h"

#define MAX17841_NO_ERROR   (char)0
#define MAX17841_ERROR      (char)1
#define MAX17841_BUSY       (char)2

/*WRITE REGISTER BLOCK*/
/*--------------------*/
/*STATUS Registers*/
#define W_STATUS_RX          (char) 0x00
#define W_STATUS_TX	         (char) 0x02
#define W_RX_INTERRUPT_EN    (char) 0x04
#define W_TX_INTERRUPT_EN    (char) 0x06
#define W_RX_INTERRUPT_FLAGS (char) 0x08
#define W_TX_INTERRUPT_FLAGS (char) 0x0A
#define W_CONFIGURATION_1    (char) 0x0C
#define W_CONFIGURATION_2    (char) 0x0E
#define W_CONFIGURATION_3    (char) 0x10

/*ACTION REGISTER BLOCK*/
/*--------------------*/
#define A_CLR_TX_BUF           (char) 0x20
#define A_RX_RD_POINTER        (char) 0x91
#define A_RX_RD_NXT_MSG        (char) 0x93
#define A_WR_NXT_LD_Q          (char) 0xB0
#define A_WR_LD_Q              (char) 0xC0
#define A_CLR_RX_BUF           (char) 0xE0

/*READ REGISTER BLOCK*/
/*-------------------*/
#define R_STATUS_RX          (char) 0x01
#define R_STATUS_TX	         (char) 0x03
#define R_RX_INTERRUPT_EN    (char) 0x05
#define R_TX_INTERRUPT_EN    (char) 0x07
#define R_RX_INTERRUPT_FLAGS (char) 0x09
#define R_TX_INTERRUPT_FLAGS (char) 0x0B
#define R_CONFIGURATION_1    (char) 0x0D
#define R_CONFIGURATION_2    (char) 0x0F
#define R_CONFIGURATION_3    (char) 0x11
#define R_FMEA               (char) 0x13
#define R_MODEL              (char) 0x15
#define R_VERSION            (char) 0x17
#define R_RX_BYTE            (char) 0x19
#define R_RX_SPACE           (char) 0x1B
#define R_TX_QUEUE_SELECTS   (char) 0x95
#define R_RX_READ_POINTER    (char) 0x97
#define R_RX_WRITE_POINTER   (char) 0x99
#define R_RX_NEXT_MESSAGE    (char) 0x9B


/**
 * @brief MAX17841 SPI to MAXBMS_UART SAFEMON Bridge
 *
 * @details The MAX17841 is a SPI to Differential UART Safe Monitoring Bridge
 * which allows to communicate via isolated channels to a daisy chain of
 * MAX BMS ICs. The Daisy chain can address up to 32 devices featuring MAX17823 (12ch)
 * up to MAX17852 MAX17853 MAX17854 (14ch) devices.
 * The IC also does automatic Keep-Alive generation error checking and has a watchdog
 * which puts the device into SAFEMON mode which is capable of controlling the whole 
 * daisy chain without the controller and bring the system into a safe state.
 * <br>https://www.maximintegrated.com/en/products/power/battery-management/MAX17841.html
 *
 * @code
 * #include "mbed.h"
 * #include "MAX17841.h"
 *
 * // SPI Master 17851
 * SPI spi0(P0_4 P0_5 P0_6 P0.7);
 *
* // SPI Slave 17851 (if using DUAL UART)
 * SPI spi1(P1_0 P1_1 P1_2 P1_3);
 *
 * MAX17841 M_MAX17841(spi0);
 * MAX17841 S_MAX17841(spi1);
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

class MAX17841
{
public:
    /**
     * MAX17841 constructor
     *
     * @param extspi Externally (outside of class) defined SPI interface
     * @param AlrtPin Connection of Alert Pin to Controller
     */
    MAX17841(SPI &extspi, PinName AlrtPin);

    /**
     * MAX17841 destructor
     */
    ~MAX17841();

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
    char DC_writeAll(char reg, unsigned short regval);

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
    char DC_writeDevice(char u5Device, char reg, unsigned short regval);

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
    char DC_readAll_RB(unsigned short* values);

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

    int itr;
    
    SPI &myspi;             /*internally used name for generic SPI interface*/
    DigitalIn *AlrtPin;     /*internally used name for Alert pin of MAX17841*/
};

#endif
