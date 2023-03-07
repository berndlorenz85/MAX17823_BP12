
/*******************************************************************************
 * Copyright (C) 2018 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */

#include "MAX77650.h"

#define SBB2_TV_MIN_MV 800
#define SBB2_TV_MAX_MV 3950
#define SBB2_TV_LSB_MV 50

//******************************************************************************
MAX77650::MAX77650(I2C &i2c, PinName pwrHldPin, int addr) :
    i2c(i2c)
{
    devAddr = addr;
    pwrHld = new DigitalOut(pwrHldPin, 1);
}

//******************************************************************************
MAX77650::~MAX77650()
{
    delete pwrHld;
}

//******************************************************************************
int MAX77650::readReg(reg_t reg, char *val)
{
    char wbuf[] = { reg };

    if (!(i2c.write(devAddr, wbuf, sizeof(wbuf), true))) {
        if (!(i2c.read(devAddr, val, 1))) {
            return MAX77650_NO_ERROR;
        }
    }

    return MAX77650_ERROR;
}

//******************************************************************************
int MAX77650::writeReg(reg_t reg, char val)
{
    char wbuf[] = { reg, val };

    if (!(i2c.write(devAddr, wbuf, sizeof(wbuf)))) {
        return MAX77650_NO_ERROR;
    }

    return MAX77650_ERROR;
}

//******************************************************************************
int MAX77650::writeReg(const char *buf, int len)
{
    if (!(i2c.write(devAddr, buf, len))) {
        return MAX77650_NO_ERROR;
    }

    return MAX77650_ERROR;
}

//******************************************************************************
int MAX77650::cid(void) {
    char rbuf[1];

    if (readReg(CID, rbuf)) {
        return MAX77650_ERROR;
    }

    return *rbuf;
}

//******************************************************************************
int MAX77650::enableLDO(void)
{
    char ade_flag[1];

    if (readReg(CNFG_LDO_B, ade_flag)) {
        return MAX77650_ERROR;
    }

    *ade_flag &= 0x08;

    return writeReg(CNFG_LDO_B, *ade_flag | 0x06);
}

//******************************************************************************
int MAX77650::disableLDO(void)
{
    char ade_flag[1];

    if (readReg(CNFG_LDO_B, ade_flag)) {
        return MAX77650_ERROR;
    }

    *ade_flag &= 0x08;

    return writeReg(CNFG_LDO_B, *ade_flag | 0x04);
}

//******************************************************************************
int MAX77650::setSBB2VoltageMV(uint32_t tv_mv)
{
    uint32_t tv_code;
    char ip_code[1];

    if ((tv_mv > SBB2_TV_MAX_MV) || (tv_mv < SBB2_TV_MIN_MV) || (tv_mv % SBB2_TV_LSB_MV)) {
        return MAX77650_ERROR;
    }

    tv_code = (tv_mv - SBB2_TV_MIN_MV) / SBB2_TV_LSB_MV;

    if (readReg(CNFG_SBB2_A, ip_code)) {
        return MAX77650_ERROR;
    }

    *ip_code &= 0xC0;

    return writeReg(CNFG_SBB2_A, tv_code | *ip_code);
}

//******************************************************************************
int MAX77650::setSBB2Voltage(float tv_v)
{
    return setSBB2VoltageMV(tv_v * 1000);
}

//******************************************************************************
void MAX77650::assertPowerHold(void)
{
    pwrHld->write(1);
}

//******************************************************************************
void MAX77650::deassertPowerHold(void)
{
    pwrHld->write(0);
}
