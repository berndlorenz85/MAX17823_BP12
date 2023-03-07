#include "mbed.h"
#include "MAX17055_BatteryCheck.h"
#include "LowLevel/MAX17055/MAX17055.h"

extern I2C i2c2;

MAX17055 max17055(i2c2);

void MAX17055_BatteryCheck_Init(void)
{
    int status;

    // Set sense resistor value
    max17055.init(0.05f);

    // Print status
    max17055.status(&status);
    printf("MAX17055 status: %04X\r\n", (uint16_t)status);
}

void MAX17055_BatteryCheck_Loop(void)
{
    int value, max, min;
    float f_value, f_max, f_min;

    // Print raw hex register contents
    max17055.v_cell(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.avg_v_cell(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.max_min_volt(&max, &min);
    printf("0x%02X 0x%02X\r\n", (uint8_t)max, (uint8_t)min);

    max17055.current(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.avg_current(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.max_min_curr(&max, &min);
    printf("0x%02X 0x%02X\r\n", (uint8_t)max, (uint8_t)min);

    max17055.temp(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.avg_ta(&value);
    printf("0x%04X ", (uint16_t)value);
    max17055.max_min_temp(&max, &min);
    printf("0x%02X 0x%02X\r\n", (uint8_t)max, (uint8_t)min);
    printf("\r\n");

    // Print formatted voltage, current and temperature values
    max17055.v_cell(&f_value);
    printf("%6.3fV ", f_value);
    max17055.avg_v_cell(&f_value);
    printf("%6.3fVavg ", f_value);
    max17055.max_min_volt(&f_max, &f_min);
    printf("%6.3fVmax %6.3fVmin\r\n", f_max, f_min);

    max17055.current(&f_value);
    printf("%6.3fI ", f_value);
    max17055.avg_current(&f_value);
    printf("%6.3fIavg ", f_value);
    max17055.max_min_curr(&f_max, &f_min);
    printf("%6.3fImax %6.3fImin\r\n", f_max, f_min);

    max17055.temp(&f_value);
    printf("%6.3fC ", f_value);
    max17055.avg_ta(&f_value);
    printf("%6.3fCavg ", f_value);
    max17055.max_min_temp(&f_max, &f_min);
    printf("%6.3fCmax %6.3fCmin\r\n", f_max, f_min);
    printf("\r\n");
}
