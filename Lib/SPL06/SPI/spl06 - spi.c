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

typedef struct {
    uint8_t cmd;
    uint8_t data;
} TransmitBuf_t;

SPL06_ALLCONFIG_t spl06Allconfig;

SPL06_RESULT_t SPL06_Initialize(SPL06_CONFIG_t *spl06Config) {
    TransmitBuf_t transmitBuf;
    HAL_StatusTypeDef spiRes;

    //IO settings & hardware settings & config copy
    spl06Allconfig.hspi = spl06Config->hspi;
    spl06Allconfig.NSS_GPIO_Port_v = spl06Config->NSS_GPIO_Port_v;
    spl06Allconfig.NSS_Pin_v = spl06Config->NSS_Pin_v;
    spl06Allconfig.measurementMode = spl06Config->measurementMode;
    spl06Allconfig.fifoEnable = spl06Config->fifoEnable;
    spl06Allconfig.temperatureRate = spl06Config->temperatureRate;
    spl06Allconfig.pressureRate = spl06Config->pressureRate;
    spl06Allconfig.temperatureOversamplingRate = spl06Config->temperatureOversamplingRate;
    spl06Allconfig.pressureOversamplingRate = spl06Config->pressureOversamplingRate;

    //Reset
    transmitBuf.cmd = SPL06_RESET;
    transmitBuf.data = 0x89;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }
    HAL_Delay(200);

    transmitBuf.cmd = SPL06_RESET;
    transmitBuf.data = 0x89;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }
    HAL_Delay(200);

    //Who Am I Check SPL06
    transmitBuf.cmd = SPL06_ID | SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    if (transmitBuf.data != 0x10) {
        printf("SPL06 Init Fail! Who Am i : 0x%02x\r\n", transmitBuf.data);
        return SPL06_RESULT_FAIL;
    }
    printf("SPL06 Init Success! Who Am i : 0x%02x\r\n", transmitBuf.data);
}

SPL06_RESULT_t SPL06_Config(SPL06_CONFIG_t *spl06Config) {
    uint8_t transmitBuf[19] = {0};
    HAL_StatusTypeDef spiRes;

    //IO settings & hardware settings & config copy
    spl06Allconfig.hspi = spl06Config->hspi;
    spl06Allconfig.NSS_GPIO_Port_v = spl06Config->NSS_GPIO_Port_v;
    spl06Allconfig.NSS_Pin_v = spl06Config->NSS_Pin_v;
    spl06Allconfig.measurementMode = spl06Config->measurementMode;
    spl06Allconfig.fifoEnable = spl06Config->fifoEnable;
    spl06Allconfig.temperatureRate = spl06Config->temperatureRate;
    spl06Allconfig.pressureRate = spl06Config->pressureRate;
    spl06Allconfig.temperatureOversamplingRate = spl06Config->temperatureOversamplingRate;
    spl06Allconfig.pressureOversamplingRate = spl06Config->pressureOversamplingRate;

    //read COEF
    transmitBuf[0] = SPL06_COEF | SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 19,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

#ifdef DEBUG_PRINT
    printf("SPL06: COEF (c0,c1): %02x,%02x,%02x.\r\n", transmitBuf[1],transmitBuf[2],transmitBuf[3]);
#endif

    spl06Allconfig.c0 = ((int16_t)transmitBuf[1]<<4 ) | ((transmitBuf[2]&0xF0)>>4);
    spl06Allconfig.c0 = (spl06Allconfig.c0 & 0x800)?(spl06Allconfig.c0|0xF800):spl06Allconfig.c0;
    spl06Allconfig.c1 = ((int16_t)(transmitBuf[2]&0x0F)<<8 ) | transmitBuf[3];
    spl06Allconfig.c1 = (spl06Allconfig.c1 & 0x800)?(spl06Allconfig.c1|0xF800):spl06Allconfig.c1;
    spl06Allconfig.c00 = ((int32_t)transmitBuf[4] << 12) | ((int16_t)transmitBuf[5] << 4) | ((transmitBuf[6] & 0xF0) >> 4);
    spl06Allconfig.c00 = (spl06Allconfig.c00 & 0x80000)?(spl06Allconfig.c00|0xFFF00000):spl06Allconfig.c00;
    spl06Allconfig.c10 = (((int32_t)transmitBuf[6] & 0x0F) << 16) | ((int16_t)transmitBuf[7] << 8) | transmitBuf[8];
    spl06Allconfig.c10 = (spl06Allconfig.c10 & 0x80000)?(spl06Allconfig.c10|0xFFF00000):spl06Allconfig.c10;
    spl06Allconfig.c01 = (int16_t)transmitBuf[9] << 8 | transmitBuf[10];
    spl06Allconfig.c01 = (spl06Allconfig.c01 & 0x800)?(spl06Allconfig.c01|0xF800):spl06Allconfig.c01;
    spl06Allconfig.c11 = (int16_t)transmitBuf[11] << 8 | transmitBuf[12];
    spl06Allconfig.c11 = (spl06Allconfig.c11 & 0x800)?(spl06Allconfig.c11|0xF800):spl06Allconfig.c11;
    spl06Allconfig.c20 = (int16_t)transmitBuf[13] << 8 | transmitBuf[14];
    spl06Allconfig.c20 = (spl06Allconfig.c20 & 0x800)?(spl06Allconfig.c20|0xF800):spl06Allconfig.c20;
    spl06Allconfig.c21 = (int16_t)transmitBuf[15] << 8 | transmitBuf[16];
    spl06Allconfig.c21 = (spl06Allconfig.c21 & 0x800)?(spl06Allconfig.c21|0xF800):spl06Allconfig.c21;
    spl06Allconfig.c30 = (int16_t)transmitBuf[17] << 8 | transmitBuf[18];
    spl06Allconfig.c30 = (spl06Allconfig.c30 & 0x800)?(spl06Allconfig.c30|0xF800):spl06Allconfig.c30;
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
    transmitBuf[0] = SPL06_PSR_CFG;
    transmitBuf[1] = spl06Allconfig.pressureRate | spl06Allconfig.pressureOversamplingRate;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //read COEF_SRCE what are Temperature coefficients based on, save return value (transmitBuf[1] & 0x80);
    transmitBuf[0] = SPL06_COEF_SRCE|SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);
    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config Temperature measurement rate & resolution
    transmitBuf[0] = SPL06_TMP_CFG;
    //Temperature sensor selected in Reset value may be not adapt to coef.TMP_EXT use external sensor(transmitBuf[1] = transmitBuf[1] & 0x80) directly.
    //transmitBuf[1] = transmitBuf[1] & 0x80;
    //transmitBuf[1] = transmitBuf[1] | spl06Allconfig.temperatureRate | spl06Allconfig.temperatureOversamplingRate;
    transmitBuf[1] = spl06Allconfig.temperatureRate | spl06Allconfig.temperatureOversamplingRate | 0x80;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config FIFO
    transmitBuf[0] = SPL06_CFG_REG|SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    transmitBuf[0] = SPL06_CFG_REG;
    transmitBuf[1] = transmitBuf[1]|0x0c|spl06Allconfig.fifoEnable<<1; // open t-shift p-shift
    if(spl06Allconfig.temperatureOversamplingRate <=SPL06_TMP_PRC_8)
    {
        transmitBuf[1] &= ~SPL06_CFG_T_SHIFT;
    }
    if(spl06Allconfig.pressureOversamplingRate <=SPL06_PM_PRC_8)
    {
        transmitBuf[1] &= ~SPL06_CFG_P_SHIFT;
    }
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);
    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    //config measurement mode
    transmitBuf[0] = SPL06_MEAS_CFG;
    transmitBuf[1] = spl06Allconfig.measurementMode;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 2,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }
}

SPL06_RESULT_t SPL06_Update_FIFO(SPL06_DATA_t *spl06Data) {
    HAL_StatusTypeDef spiRes;
    uint8_t transmitBuf[4] = {0};
    int32_t rawValue = 0;

    transmitBuf[0] = SPL06_PSR_B2| SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 4,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);

    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t)transmitBuf[1]<<16) | ((int32_t)transmitBuf[2]<<8) | (int32_t)transmitBuf[3];

    if (rawValue == 0x800000) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = (rawValue&0x800000)?(0xFF000000|rawValue):rawValue;
    if((rawValue & 1) == 0)
    {
//        spl06Allconfig.T_raw_sc =  rawValue / spl06Allconfig.KT;
//        spl06Allconfig.T_comp = spl06Allconfig.c0 * 0.5f + spl06Allconfig.c1 * spl06Allconfig.T_raw_sc;
//        printf("T_comp: %f, T_raw_sc: %f\r\n",spl06Allconfig.T_comp ,spl06Allconfig.T_raw_sc);
    }

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
        spl06Allconfig.T_raw_sc =  rawValue / spl06Allconfig.KT;
        spl06Allconfig.T_comp = spl06Allconfig.c0 * 0.5f + spl06Allconfig.c1 * spl06Allconfig.T_raw_sc;
        //temperature measurement
    }

    spl06Data->temperature = spl06Allconfig.T_comp;
    spl06Data->pressure = spl06Allconfig.P_comp;
    spl06Data->altitude = (44330.0 *(1.0-pow((float)(spl06Data->pressure) / 101325.0,1.0/5.255)));
    return SPL06_RESULT_OK;
}

SPL06_RESULT_t SPL06_UpdatePsr(SPL06_DATA_t *spl06Data) {
    HAL_StatusTypeDef spiRes;
    uint8_t transmitBuf[4] = {0};
    int32_t rawValue = 0;

    transmitBuf[0] = SPL06_PSR_B2| SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 4,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);
    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t)transmitBuf[1]<<16) | ((int32_t)transmitBuf[2]<<8) | (int32_t)transmitBuf[3];
    rawValue = (rawValue&0x800000)?(0xFF000000|rawValue):rawValue;

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
}

SPL06_RESULT_t SPL06_UpdateTemp(SPL06_DATA_t *spl06Data) {
    HAL_StatusTypeDef spiRes;
    uint8_t transmitBuf[4] = {0};
    int32_t rawValue = 0;

    transmitBuf[0] = SPL06_TMP_B2| SPL06_READ_MASK;
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(spl06Allconfig.hspi, (uint8_t *) &transmitBuf, (uint8_t *) &transmitBuf, 4,
                                     0xffff);
    HAL_GPIO_WritePin(spl06Allconfig.NSS_GPIO_Port_v, spl06Allconfig.NSS_Pin_v, 1);
    if (spiRes != HAL_OK) {
        return SPL06_RESULT_FAIL;
    }

    rawValue = ((int32_t)transmitBuf[1]<<16) | ((int32_t)transmitBuf[2]<<8) | (int32_t)transmitBuf[3];
    rawValue = (rawValue&0x800000)?(0xFF000000|rawValue):rawValue;


    if (rawValue == 0x800000 | rawValue == 0x000000) {
        return SPL06_RESULT_FAIL;
    }

    spl06Allconfig.T_raw_sc =  rawValue / spl06Allconfig.KT;
    spl06Allconfig.T_comp = spl06Allconfig.c0 * 0.5f + spl06Allconfig.c1 * spl06Allconfig.T_raw_sc;
    spl06Data->temperature = spl06Allconfig.T_comp;
}

SPL06_RESULT_t SPL06_Update(SPL06_DATA_t *spl06Data) {
    SPL06_UpdateTemp(spl06Data);
    SPL06_UpdatePsr(spl06Data);
    spl06Data->altitude = (44330.0 *(1.0-pow((float)(spl06Data->pressure) / 101325.0,1.0/5.255)));
}