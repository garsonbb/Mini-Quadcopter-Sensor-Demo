/**
* @file         MPU6050.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,11
* @version	    v1.0
*/
#include "mpu6050.h"
#include "retarget.h"

//#define DEBUG_PRINT

typedef struct
{
    uint8_t cmd;
    uint8_t data;
}TransmitBuf_t;

HAL_StatusTypeDef _MPU6050_IIC_Write_Bit(uint8_t regAddr, uint8_t data);

HAL_StatusTypeDef _MPU6050_IIC_Read_Bit(uint8_t regAddr, uint8_t *data);

HAL_StatusTypeDef _MPU6050_IIC_Read_Bits(uint8_t regAddr, uint8_t *data, uint8_t length);

MPU6050_ALLCONFIG_t MPU6050Allconfig;

MPU6050_RESULT_t MPU6050_Initialize(MPU6050_CONFIG_t *MPU6050Config) {
    uint8_t resTmp = 0;
    HAL_StatusTypeDef i2cRes;

    //IO settings & hardware settings
    MPU6050Allconfig.hi2c=MPU6050Config->hi2c;
    MPU6050Allconfig.ACCEL_SCALE = MPU6050Config->ACCEL_SCALE;
    MPU6050Allconfig.GYRO_SCALE = MPU6050Config->GYRO_SCALE;

    //Reset
	i2cRes = _MPU6050_IIC_Write_Bit(MPU6050_PWR_MGMT_1, 0x80);
    HAL_Delay(100);
    i2cRes = _MPU6050_IIC_Write_Bit(MPU6050_PWR_MGMT_1, 0x00);//按datasheet来说，会Reset位自动清0，但是不知是不是我的模块问题，不手动清0，就不能读取6轴和温度寄存器。

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }
    HAL_Delay(100);

    //Who Am I Check MPU

	i2cRes = _MPU6050_IIC_Read_Bit(MPU6050_WHO_AM_I, &resTmp);

    if (resTmp != 0x68 || i2cRes != HAL_OK) {
        printf("MPU6050 Init Fail! Who Am i : 0x%02x\r\n", resTmp);
        return MPU6050_RESULT_FAIL;
    }
    printf("MPU6050 Init Success! Who Am i : 0x%02x\r\n", resTmp);
}

MPU6050_RESULT_t MPU6050_Config(MPU6050_CONFIG_t *MPU6050Config) {
	uint8_t tmpCmd = 0;
	uint8_t tmpData = 0;
    HAL_StatusTypeDef i2cRes;

    //IO settings & hardware settings
    MPU6050Allconfig.hi2c=MPU6050Config->hi2c;
    MPU6050Allconfig.ACCEL_SCALE = MPU6050Config->ACCEL_SCALE;
    MPU6050Allconfig.GYRO_SCALE = MPU6050Config->GYRO_SCALE;

    //clear MPU6050Allconfig
    for (int i = 0; i < 3; ++i) {
        MPU6050Allconfig.ACCEL_OFFSET[i] = 0;
    }
    for (int i = 0; i < 3; ++i) {
        MPU6050Allconfig.GYRO_OFFSET[i] = 0;
    }

    /* MPU6050_Set_Accel_Scale ---------------------------------------------------------*/
    tmpData = MPU6050Allconfig.ACCEL_SCALE;
    if (tmpData == ACCEL_SCALE_2G) {//2G
        MPU6050Allconfig.ARES = 2.0f;
    } else if (tmpData == ACCEL_SCALE_4G) {//4G
        MPU6050Allconfig.ARES = 4.0f;
    } else if (tmpData == ACCEL_SCALE_8G) {//8G
        MPU6050Allconfig.ARES = 8.0f;
    } else if (tmpData == ACCEL_SCALE_16G) {//16G
        MPU6050Allconfig.ARES = 16.0f;
    }

    tmpCmd = MPU6050_ACCEL_CFG;      //Save xyz Accelself-test Value
	i2cRes = _MPU6050_IIC_Read_Bit(tmpCmd, &tmpData);

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }

    tmpCmd = MPU6050_ACCEL_CFG;
    tmpData = tmpData | MPU6050Allconfig.ACCEL_SCALE;
    i2cRes = _MPU6050_IIC_Write_Bit(tmpCmd, tmpData);

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }

/* MPU6050_GYRO_CONFIG ---------------------------------------------------------*/

    tmpData = MPU6050Allconfig.GYRO_SCALE;
    if (tmpData == GYRO_SCALE_250dps) {//250dps
        MPU6050Allconfig.GRES = 250.f/57.3f;
    } else if (tmpData == GYRO_SCALE_500dps) {//500dps
        MPU6050Allconfig.GRES = 500.f/57.3f;
    } else if (tmpData == GYRO_SCALE_1000dps) {//1000dps
        MPU6050Allconfig.GRES = 1000.f/57.3f;
    } else if (tmpData == GYRO_SCALE_2000dps) {//2000dps
        MPU6050Allconfig.GRES = 2000.f/57.3f;
    }

    tmpCmd = MPU6050_GYRO_CFG;      //Save xyz Accelself-test Value
	i2cRes = _MPU6050_IIC_Read_Bit(tmpCmd, &tmpData);

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }

    tmpCmd = MPU6050_GYRO_CFG;
    tmpData = tmpData | MPU6050Allconfig.ACCEL_SCALE;
	i2cRes = _MPU6050_IIC_Write_Bit(tmpCmd, tmpData);

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }

    return MPU6050_RESULT_OK;
}

MPU6050_RESULT_t MPU6050_Calibrate(){
    MPU6050_DATA_t MPU6050_TEMP;
    float ax_sum=0,ay_sum=0,az_sum=0;
    float gx_sum=0,gy_sum=0,gz_sum=0;

    /* MPU6050_Accel_Gyro_Calibration---------------------------------------------------------*/

    for(int i=0;i<CALIBRATE_TIMES;i++){//Take samples by the number of CALIBRATE_TIME
        MPU6050_Get7DOF(&MPU6050_TEMP);
        ax_sum += MPU6050_TEMP.accel[0];
        ay_sum += MPU6050_TEMP.accel[1];
        az_sum += MPU6050_TEMP.accel[2] - 1;

        gx_sum += MPU6050_TEMP.gyro[0];
        gy_sum += MPU6050_TEMP.gyro[1];
        gz_sum += MPU6050_TEMP.gyro[2];
        HAL_Delay(2);
    }
    MPU6050Allconfig.ACCEL_OFFSET[0] = ax_sum/CALIBRATE_TIMES;
    MPU6050Allconfig.ACCEL_OFFSET[1] = ay_sum/CALIBRATE_TIMES;
    MPU6050Allconfig.ACCEL_OFFSET[2] = az_sum/CALIBRATE_TIMES;

    MPU6050Allconfig.GYRO_OFFSET[0] = gx_sum/CALIBRATE_TIMES;
    MPU6050Allconfig.GYRO_OFFSET[1] = gy_sum/CALIBRATE_TIMES;
    MPU6050Allconfig.GYRO_OFFSET[2] = gz_sum/CALIBRATE_TIMES;

    return MPU6050_RESULT_OK;
}

MPU6050_RESULT_t MPU6050_Get7DOF(MPU6050_DATA_t *MPU6050_DATA){
    uint8_t transmitBuf[14];
    HAL_StatusTypeDef i2cRes;

    /* MPU6050_Get_Accel_Gyro_and_Temp ---------------------------------------------------------*/
	i2cRes = _MPU6050_IIC_Read_Bits(MPU6050_ACCEL_XOUT_H,transmitBuf,14);

    if (i2cRes != HAL_OK) {
        return MPU6050_RESULT_FAIL;
    }

    for(int i=0;i<3;i++){
        MPU6050_DATA->accel_raw[i] = (uint16_t)transmitBuf[2*i] << 8 | transmitBuf[2*i+1];
        MPU6050_DATA->gyro_raw[i] = (uint16_t)transmitBuf[2*i+8] << 8 | transmitBuf[2*i+9];
        MPU6050_DATA->accel[i] = ((int16_t)MPU6050_DATA->accel_raw[i] * MPU6050Allconfig.ARES)/32768.0f - MPU6050Allconfig.ACCEL_OFFSET[i];
        MPU6050_DATA->gyro[i] = ((int16_t)MPU6050_DATA->gyro_raw[i] * MPU6050Allconfig.GRES)/32768.0f - MPU6050Allconfig.GYRO_OFFSET[i];
    }
    MPU6050_DATA->temperature_raw = ((uint16_t)transmitBuf[6] << 8) | transmitBuf[7];
    MPU6050_DATA->temperature = (float)((int16_t)MPU6050_DATA->temperature_raw) / 340.f + 36.53f;
    return MPU6050_RESULT_OK;
}



HAL_StatusTypeDef _MPU6050_IIC_Write_Bit(uint8_t regAddr, uint8_t data) {
    return HAL_I2C_Mem_Write(MPU6050Allconfig.hi2c, MPU6050_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);
}

HAL_StatusTypeDef _MPU6050_IIC_Read_Bit(uint8_t regAddr, uint8_t *data) {
    return HAL_I2C_Mem_Read(MPU6050Allconfig.hi2c, MPU6050_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, data, 1, 0xff);
}

HAL_StatusTypeDef _MPU6050_IIC_Read_Bits(uint8_t regAddr, uint8_t *data, uint8_t length) {
    return HAL_I2C_Mem_Read(MPU6050Allconfig.hi2c, MPU6050_DEVADDR, regAddr, I2C_MEMADD_SIZE_8BIT, data, length, 0xff);
}
