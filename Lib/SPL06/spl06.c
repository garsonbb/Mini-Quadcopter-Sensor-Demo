/**
* @file         SPL06.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,12
* @version	    v1.0
*/
#include "spl06.h"
#include "math.h"
#include "retarget.h"

//#define DEBUG_PRINT


HAL_StatusTypeDef _SPL06_IIC_Write_Bit(uint8_t regAddr, uint8_t data);

HAL_StatusTypeDef _SPL06_IIC_Read_Bit(uint8_t regAddr, uint8_t *data);

HAL_StatusTypeDef _SPL06_IIC_Read_Bits(uint8_t regAddr, uint8_t *data, uint8_t length);

SPL06_ALLCONFIG_t spl06Allconfig;

SPL06_RESULT_t SPL06_Initialize(SPL06_CONFIG_t *spl06Config) {
    HAL_StatusTypeDef i2cRes;
    uint8_t resTmp = 0;
    uint32_t tickstart=0;
    uint32_t i =0;

    //IO settings & hardware settings & config copy
    spl06Allconfig.hi2c = spl06Config->hi2c;
    spl06Allconfig.measurementMode = spl06Config->measurementMode;
    spl06Allconfig.fifoEnable = spl06Config->fifoEnable;
    spl06Allconfig.temperatureRate = spl06Config->temperatureRate;
    spl06Allconfig.pressureRate = spl06Config->pressureRate;
    spl06Allconfig.temperatureOversamplingRate = spl06Config->temperatureOversamplingRate;
    spl06Allconfig.pressureOversamplingRate = spl06Config->pressureOversamplingRate;

    //Reset
    i2cRes = _SPL06_IIC_Write_Bit(SPL06_RESET, 0x89);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }
    HAL_Delay(300);
    tickstart = HAL_GetTick();
    do
    {
        _SPL06_IIC_Read_Bit(SPL06_MEAS_CFG,&resTmp);//读取气压计启动状态
        if(HAL_GetTick()-tickstart>200)
            return SPL06_RESULT_FAIL;
    }
    while((resTmp&SPL06_MEAS_COEF_RDY)!=SPL06_MEAS_COEF_RDY);

    tickstart = HAL_GetTick();
    do
    {
        _SPL06_IIC_Read_Bit(SPL06_MEAS_CFG,&resTmp);//读取气压计启动状态
        printf("%x\r\n",resTmp);
        if(HAL_GetTick()-tickstart>200)
            return SPL06_RESULT_FAIL;
    }
    while((resTmp&SPL06_MEAS_SENSOR_RDY)!=SPL06_MEAS_SENSOR_RDY);

    //Who Am I Check SPL06
    i2cRes = _SPL06_IIC_Read_Bit(SPL06_ID, &resTmp);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    if (resTmp != 0x10) {
        printf("SPL06 Init Fail! Who Am i : 0x%02x\r\n", resTmp);
        return SPL06_RESULT_FAIL;
    }
    printf("SPL06 Init Success! Who Am i : 0x%02x\r\n", resTmp);

    return SPL06_RESULT_OK;
}

SPL06_RESULT_t SPL06_Config(SPL06_CONFIG_t *spl06Config) {
    uint8_t transmitBuf[18] = {0};
    uint8_t resTmp = 0;
    HAL_StatusTypeDef i2cRes;


    //IO settings & hardware settings & config copy
    spl06Allconfig.hi2c = spl06Config->hi2c;
    spl06Allconfig.measurementMode = spl06Config->measurementMode;
    spl06Allconfig.fifoEnable = spl06Config->fifoEnable;
    spl06Allconfig.temperatureRate = spl06Config->temperatureRate;
    spl06Allconfig.pressureRate = spl06Config->pressureRate;
    spl06Allconfig.temperatureOversamplingRate = spl06Config->temperatureOversamplingRate;
    spl06Allconfig.pressureOversamplingRate = spl06Config->pressureOversamplingRate;

    //read COEF
    i2cRes = _SPL06_IIC_Read_Bits(SPL06_COEF, transmitBuf, 18);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

#ifdef DEBUG_PRINT
    printf("SPL06: COEF (c0,c1): %02x,%02x,%02x.\r\n", transmitBuf[0],transmitBuf[1],transmitBuf[2]);
#endif

    spl06Allconfig.c0 = ((int16_t) transmitBuf[0] << 4) | ((transmitBuf[1] & 0xF0) >> 4);
    spl06Allconfig.c0 = (spl06Allconfig.c0 & 0x800) ? (spl06Allconfig.c0 | 0xF800) : spl06Allconfig.c0;
    spl06Allconfig.c1 = ((int16_t) (transmitBuf[1] & 0x0F) << 8) | transmitBuf[2];
    spl06Allconfig.c1 = (spl06Allconfig.c1 & 0x800) ? (spl06Allconfig.c1 | 0xF800) : spl06Allconfig.c1;
    spl06Allconfig.c00 =
            ((int32_t) transmitBuf[3] << 12) | ((int16_t) transmitBuf[4] << 4) | ((transmitBuf[5] & 0xF0) >> 4);
    spl06Allconfig.c00 = (spl06Allconfig.c00 & 0x80000) ? (spl06Allconfig.c00 | 0xFFF00000) : spl06Allconfig.c00;
    spl06Allconfig.c10 = (((int32_t) transmitBuf[5] & 0x0F) << 16) | ((int16_t) transmitBuf[6] << 8) | transmitBuf[7];
    spl06Allconfig.c10 = (spl06Allconfig.c10 & 0x80000) ? (spl06Allconfig.c10 | 0xFFF00000) : spl06Allconfig.c10;
    spl06Allconfig.c01 = (int16_t) transmitBuf[8] << 8 | transmitBuf[9];
    spl06Allconfig.c01 = (spl06Allconfig.c01 & 0x800) ? (spl06Allconfig.c01 | 0xF800) : spl06Allconfig.c01;
    spl06Allconfig.c11 = (int16_t) transmitBuf[10] << 8 | transmitBuf[11];
    spl06Allconfig.c11 = (spl06Allconfig.c11 & 0x800) ? (spl06Allconfig.c11 | 0xF800) : spl06Allconfig.c11;
    spl06Allconfig.c20 = (int16_t) transmitBuf[12] << 8 | transmitBuf[13];
    spl06Allconfig.c20 = (spl06Allconfig.c20 & 0x800) ? (spl06Allconfig.c20 | 0xF800) : spl06Allconfig.c20;
    spl06Allconfig.c21 = (int16_t) transmitBuf[14] << 8 | transmitBuf[15];
    spl06Allconfig.c21 = (spl06Allconfig.c21 & 0x800) ? (spl06Allconfig.c21 | 0xF800) : spl06Allconfig.c21;
    spl06Allconfig.c30 = (int16_t) transmitBuf[16] << 8 | transmitBuf[17];
    spl06Allconfig.c30 = (spl06Allconfig.c30 & 0x800) ? (spl06Allconfig.c30 | 0xF800) : spl06Allconfig.c30;
#ifdef DEBUG_PRINT
    printf("SPL06: COEF: %d,%d,%lu,%lu,%d,%d,%d,%d,%d\r\n", spl06Allconfig.c0, spl06Allconfig.c1, spl06Allconfig.c00,
           spl06Allconfig.c10, spl06Allconfig.c01, spl06Allconfig.c11, spl06Allconfig.c20, spl06Allconfig.c21,
           spl06Allconfig.c30);
#endif

    //Select temperature & pressure Scale
    switch (spl06Allconfig.pressureOversamplingRate) {
        case SPL06_PM_PRC_1:
            spl06Allconfig.KP = 524288.f;
            break;
        case SPL06_PM_PRC_2:
            spl06Allconfig.KP = 1572864.f;
            break;
        case SPL06_PM_PRC_4:
            spl06Allconfig.KP = 3670016.f;
            break;
        case SPL06_PM_PRC_8:
            spl06Allconfig.KP = 7864320.f;
            break;
        case SPL06_PM_PRC_16:
            spl06Allconfig.KP = 253952.f;
            break;
        case SPL06_PM_PRC_32:
            spl06Allconfig.KP = 516096.f;
            break;
        case SPL06_PM_PRC_64:
            spl06Allconfig.KP = 1040384.f;
            break;
        case SPL06_PM_PRC_128:
            spl06Allconfig.KP = 2088960.f;
            break;
        default:
            spl06Allconfig.KP = 1.f;
    }

    switch (spl06Allconfig.temperatureOversamplingRate) {
        case SPL06_TMP_PRC_1:
            spl06Allconfig.KT = 524288.f;
            break;
        case SPL06_TMP_PRC_2:
            spl06Allconfig.KT = 1572864.f;
            break;
        case SPL06_TMP_PRC_4:
            spl06Allconfig.KT = 3670016.f;
            break;
        case SPL06_TMP_PRC_8:
            spl06Allconfig.KT = 7864320.f;
            break;
        case SPL06_TMP_PRC_16:
            spl06Allconfig.KT = 253952.f;
            break;
        case SPL06_TMP_PRC_32:
            spl06Allconfig.KT = 516096.f;
            break;
        case SPL06_TMP_PRC_64:
            spl06Allconfig.KT = 1040384.f;
            break;
        case SPL06_TMP_PRC_128:
            spl06Allconfig.KT = 2088960.f;
            break;
        default:
            spl06Allconfig.KT = 1.f;
    }

    //config Pressrue measurement rate & resolution
    i2cRes = _SPL06_IIC_Write_Bit(SPL06_PSR_CFG, spl06Allconfig.pressureRate | spl06Allconfig.pressureOversamplingRate);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //read COEF_SRCE what are Temperature coefficients based on, save return value (resTmp & 0x80);
    i2cRes = _SPL06_IIC_Read_Bit(SPL06_COEF_SRCE, &resTmp);
    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config Temperature measurement rate & resolution
    //Temperature sensor selected in Reset value may be not adapt to coef.TMP_EXT use external sensor(resTmp = resTmp & 0x80) directly.
    //resTmp = resTmp & 0x80;
    //resTmp = resTmp | spl06Allconfig.temperatureRate | spl06Allconfig.temperatureOversamplingRate;
    resTmp = spl06Allconfig.temperatureRate | spl06Allconfig.temperatureOversamplingRate | 0x80;
    i2cRes = _SPL06_IIC_Write_Bit(SPL06_TMP_CFG, resTmp);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config FIFO
    i2cRes = _SPL06_IIC_Read_Bit(SPL06_CFG_REG, &resTmp);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    transmitBuf[0] = SPL06_CFG_REG;
    transmitBuf[1] = resTmp | spl06Allconfig.fifoEnable << 1; // open t-shift p-shift
    if (spl06Allconfig.temperatureOversamplingRate > SPL06_TMP_PRC_8) {
        transmitBuf[1] |= SPL06_CFG_T_SHIFT;
    }
    if (spl06Allconfig.pressureOversamplingRate > SPL06_PM_PRC_8) {
        transmitBuf[1] |= SPL06_CFG_P_SHIFT;
    }
    i2cRes = _SPL06_IIC_Write_Bit(SPL06_CFG_REG, transmitBuf[1]);
    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config measurement mode
    i2cRes = _SPL06_IIC_Write_Bit(SPL06_MEAS_CFG, spl06Allconfig.measurementMode);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    return SPL06_RESULT_OK;
}

SPL06_RESULT_t SPL06_Update_FIFO(SPL06_DATA_t *spl06Data) // NO Recommend, Hardware problem.
{
    HAL_StatusTypeDef i2cRes;
    uint8_t transmitBuf[3] = {0};
    int32_t rawValue = 0;

    i2cRes = _SPL06_IIC_Read_Bits(SPL06_PSR_B2, transmitBuf, 3);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t) transmitBuf[0] << 16) | ((int32_t) transmitBuf[1] << 8) | (int32_t) transmitBuf[2];
//    printf("%x\r\n", rawValue);

    if (rawValue == 0x800000) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = (rawValue & 0x800000) ? (0xFF000000 | rawValue) : rawValue;

    if ((rawValue & 1) == 1) {
        spl06Allconfig.P_raw_sc = rawValue / spl06Allconfig.KP;
        spl06Allconfig.P_comp = spl06Allconfig.c00 +
                                spl06Allconfig.P_raw_sc * (spl06Allconfig.c10 + spl06Allconfig.P_raw_sc *
                                                                                (spl06Allconfig.c20 +
                                                                                 spl06Allconfig.P_raw_sc *
                                                                                 spl06Allconfig.c30)) +
                                spl06Allconfig.T_raw_sc * spl06Allconfig.c01 +
                                spl06Allconfig.T_raw_sc * spl06Allconfig.P_raw_sc *
                                (spl06Allconfig.c11 + spl06Allconfig.P_raw_sc * spl06Allconfig.c21);
        //pressure measurement
    } else {
        spl06Allconfig.T_raw_sc = rawValue / spl06Allconfig.KT;
        spl06Allconfig.T_comp = spl06Allconfig.c0 * 0.5f + spl06Allconfig.c1 * spl06Allconfig.T_raw_sc;
        //temperature measurement
    }

    spl06Data->temperature = spl06Allconfig.T_comp;
    spl06Data->pressure = spl06Allconfig.P_comp;
    spl06Data->altitude = (44330.0 * (1.0 - pow((float) (spl06Data->pressure) / 101325.0, 1.0 / 5.255)));
    return SPL06_RESULT_OK;

//    FIFO empty flag and empty data indication with returning "0x800000" is not fully
//    functional. Depending on the use case, the software driver should be programmed
//    according to solution A or solution B:
//    Solution A:
//    Read 32 values from FIFO after it is full, indicated by the FIFO full interrupt or the
//    FIFO_FULL status bit. Don't rely on the FIFO_EMPTY bit.
//    Solution B:
//    Read FIFO anytime before full indication, then the FIFO_EMPTY bit and the
//    empty indication ("0x800000") is functional.
}

SPL06_RESULT_t SPL06_UpdatePsr(SPL06_DATA_t *spl06Data) {
    HAL_StatusTypeDef i2cRes;
    uint8_t transmitBuf[3] = {0};
    int32_t rawValue = 0;

    i2cRes = _SPL06_IIC_Read_Bits(SPL06_PSR_B2, transmitBuf, 3);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t) transmitBuf[0] << 16) | ((int32_t) transmitBuf[1] << 8) | (int32_t) transmitBuf[2];
    rawValue = (rawValue & 0x800000) ? (0xFF000000 | rawValue) : rawValue;

    if (rawValue == 0x800000 | rawValue == 0x000000) {
        return SPL06_RESULT_FAIL;
    }

    spl06Allconfig.P_raw_sc = rawValue / spl06Allconfig.KP;
    spl06Allconfig.P_comp = spl06Allconfig.c00 +
                            spl06Allconfig.P_raw_sc * (spl06Allconfig.c10 + spl06Allconfig.P_raw_sc *
                                                                            (spl06Allconfig.c20 +
                                                                             spl06Allconfig.P_raw_sc *
                                                                             spl06Allconfig.c30)) +
                            spl06Allconfig.T_raw_sc * spl06Allconfig.c01 +
                            spl06Allconfig.T_raw_sc * spl06Allconfig.P_raw_sc *
                            (spl06Allconfig.c11 + spl06Allconfig.P_raw_sc * spl06Allconfig.c21);
    spl06Data->pressure = spl06Allconfig.P_comp;
    return SPL06_RESULT_OK;
}

SPL06_RESULT_t SPL06_UpdateTemp(SPL06_DATA_t *spl06Data) {
    HAL_StatusTypeDef i2cRes;
    uint8_t transmitBuf[3] = {0};
    int32_t rawValue = 0;

    i2cRes = _SPL06_IIC_Read_Bits(SPL06_TMP_B2, transmitBuf, 3);

    if (i2cRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t) transmitBuf[0] << 16) | ((int32_t) transmitBuf[1] << 8) | (int32_t) transmitBuf[2];
    rawValue = (rawValue & 0x800000) ? (0xFF000000 | rawValue) : rawValue;


    if (rawValue == 0x800000 | rawValue == 0x000000) {
        return SPL06_RESULT_FAIL;
    }

    spl06Allconfig.T_raw_sc = rawValue / spl06Allconfig.KT;
    spl06Allconfig.T_comp = spl06Allconfig.c0 * 0.5f + spl06Allconfig.c1 * spl06Allconfig.T_raw_sc;
    spl06Data->temperature = spl06Allconfig.T_comp;

    return SPL06_RESULT_OK;
}

SPL06_RESULT_t SPL06_Update(SPL06_DATA_t *spl06Data) {
    SPL06_UpdateTemp(spl06Data);
    SPL06_UpdatePsr(spl06Data);
    spl06Data->altitude = (44330.0 * (1.0 - pow((float) (spl06Data->pressure) / 101325.0, 1.0 / 5.255)));
}

HAL_StatusTypeDef _SPL06_IIC_Write_Bit(uint8_t regAddr, uint8_t data) {
    return HAL_I2C_Mem_Write(spl06Allconfig.hi2c, SPL06_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xf);
}

HAL_StatusTypeDef _SPL06_IIC_Read_Bit(uint8_t regAddr, uint8_t *data) {
    return HAL_I2C_Mem_Read(spl06Allconfig.hi2c, SPL06_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, data, 1, 0xf);
}

HAL_StatusTypeDef _SPL06_IIC_Read_Bits(uint8_t regAddr, uint8_t *data, uint8_t length) {
    return HAL_I2C_Mem_Read(spl06Allconfig.hi2c, SPL06_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, data, length, 0xf);
}