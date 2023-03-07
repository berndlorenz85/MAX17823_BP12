#include "mbed.h"
#include "MAX32620FTHR.h"
#include "MAX77650_PowerCheck.h"
#include "LowLevel/MAX77650/MAX77650.h"


// Configure GPIO voltage
MAX32620FTHR max32620fthr(MAX32620FTHR::VIO_3V3);

// LEDs connected to MAX32620
DigitalOut redLed(LED_RED, LED_OFF);
DigitalOut grnLed(LED_GREEN, LED_OFF);
DigitalOut bluLed(LED_BLUE, LED_OFF);

// I2C master 2
I2C i2c2(I2C2_SDA, I2C2_SCL);

// MAX77650 PMIC and Charger
MAX77650 max77650(i2c2);

// MAX77650 IRQ
InterruptIn max77650IntIn(P2_3);

// Configure PMIC LEDs to rotate colors
static const char ledRotate[] = {
    MAX77650::CNFG_LED0_A,
    0x44, 0x44, 0x44,
    0x17, 0x37, 0x77,
    0x01,
};

// Configure PMIC to drive green LED
static const char ledGreen[] = {
    MAX77650::CNFG_LED0_A,
    0x04, 0x44, 0x04,
    0x0F, 0x0F, 0x0F,
    0x01,
};

// Disable LED's connected to PMIC
static const char ledNone[] = {
    MAX77650::CNFG_LED0_A,
    0x04, 0x04, 0x04,
    0x0F, 0x0F, 0x0F,
    0x01,
};

// Cycle through PMIC LED's with every button push
void max77650IrqHandler(void)
{
    char irqStatus;
    static char ledPhase = 0;
    static char ledData[] = {
        MAX77650::CNFG_LED0_A,
        0x44, 0x04, 0x04,
        0x0F, 0x0F, 0x0F,
        0x01,
    };

    while (!(max77650.readReg(MAX77650::INT_GLBL, &irqStatus))) {
        if (irqStatus) {
            if (irqStatus & 0x04) {
                ledPhase++;
                switch (ledPhase % 4) {
                    case 0: ledData[1] ^= 0x40; break;
                    case 1: ledData[1] ^= 0x40; ledData[2] ^= 0x40; break;
                    case 2: ledData[2] ^= 0x40; ledData[3] ^= 0x40; break;
                    case 3: ledData[3] ^= 0x40; break;
                }
                if (!(max77650.writeReg(ledData, sizeof(ledData)))) {
                    break;
                }
            }
        } else {
            break;
        }
    }
}


void MAX77650_PowerCheck_Init(void)
{
    // Turn off Low-Dropout Linear Regulator (LDO); unused on MAX32620FTHR platform
    max77650.disableLDO();
    
    // Set SIMO Buck-Boost Regulator 2 target voltage; provides VDDIOH
    max77650.setSBB2Voltage(3.3f);

    // Turn PMIC green LED on, red and blue off
    max77650.writeReg(ledGreen, sizeof(ledGreen));

    Thread::wait(3000);

    // Configure PMIC LED's to cycle between colors
    max77650.writeReg(ledRotate, sizeof(ledRotate));

    Thread::wait(3000);

    // Turn off PMIC LED's
    max77650.writeReg(ledNone, sizeof(ledNone));


    //Initialize IRQ
    // Configure MAX77650 to generate an interrupt request when push buton SW2 is pressed
    max77650.writeReg(MAX77650::CNFG_GLBL, MAX77650::SBIA_LPM | MAX77650::DBEN_30_MS);
    max77650.writeReg(MAX77650::INTM_GLBL, MAX77650::NEN_FALL);

    // Manually invoke the MAX77650 interrupt handler to service any pending interrupts
    max77650IrqHandler();

    // Configure InterruptIn object to service MAX77650 IRQ signal
    max77650IntIn.mode(PullUp);
    max77650IntIn.fall(max77650IrqHandler);
    max77650IntIn.enable_irq();
}

void MAX77650_PowerCheck_Loop(void)
{
    static int state = 0;

    switch (state++ & 0x3) 
    {
        case 0: redLed = LED_ON; break;
        case 1: redLed = LED_OFF; grnLed = LED_ON; break;
        case 2: grnLed = LED_OFF; bluLed = LED_ON; break;
        case 3: bluLed = LED_OFF; break;
    }
}