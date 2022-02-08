/**
* @file         SPL06.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,12
* @version	    v1.0
*/
#ifndef UAV_SPL06_H
#define UAV_SPL06_H

#include "stm32f4xx_hal.h"

//气压测量速率(sample/sec),Background 模式使用
#define SPL06_PM_RATE_1          (0<<4)      //1 measurements pr. sec.
#define SPL06_PM_RATE_2          (1<<4)      //2 measurements pr. sec.
#define SPL06_PM_RATE_4          (2<<4)      //4 measurements pr. sec.
#define SPL06_PM_RATE_8          (3<<4)      //8 measurements pr. sec.
#define SPL06_PM_RATE_16         (4<<4)      //16 measurements pr. sec.
#define SPL06_PM_RATE_32         (5<<4)      //32 measurements pr. sec.
#define SPL06_PM_RATE_64         (6<<4)      //64 measurements pr. sec.
#define SPL06_PM_RATE_128        (7<<4)      //128 measurements pr. sec.

//气压重采样速率(times),Background 模式使用
#define SPL06_PM_PRC_1            0       //Sigle         kP=524288   ,3.6ms
#define SPL06_PM_PRC_2            1       //2 times       kP=1572864  ,5.2ms
#define SPL06_PM_PRC_4            2       //4 times       kP=3670016  ,8.4ms
#define SPL06_PM_PRC_8            3       //8 times       kP=7864320  ,14.8ms
#define SPL06_PM_PRC_16           4       //16 times      kP=253952   ,27.6ms
#define SPL06_PM_PRC_32           5       //32 times      kP=516096   ,53.2ms
#define SPL06_PM_PRC_64           6       //64 times      kP=1040384  ,104.4ms
#define SPL06_PM_PRC_128          7       //128 times     kP=2088960  ,206.8ms

//温度测量速率(sample/sec),Background 模式使用
#define SPL06_TMP_RATE_1         (0<<4)      //1 measurements pr. sec.
#define SPL06_TMP_RATE_2         (1<<4)      //2 measurements pr. sec.
#define SPL06_TMP_RATE_4         (2<<4)      //4 measurements pr. sec.
#define SPL06_TMP_RATE_8         (3<<4)      //8 measurements pr. sec.
#define SPL06_TMP_RATE_16        (4<<4)      //16 measurements pr. sec.
#define SPL06_TMP_RATE_32        (5<<4)      //32 measurements pr. sec.
#define SPL06_TMP_RATE_64        (6<<4)      //64 measurements pr. sec.
#define SPL06_TMP_RATE_128       (7<<4)      //128 measurements pr. sec.

//温度重采样速率(times),Background 模式使用
#define SPL06_TMP_PRC_1           0       //Sigle
#define SPL06_TMP_PRC_2           1       //2 times
#define SPL06_TMP_PRC_4           2       //4 times
#define SPL06_TMP_PRC_8           3       //8 times
#define SPL06_TMP_PRC_16          4       //16 times
#define SPL06_TMP_PRC_32          5       //32 times
#define SPL06_TMP_PRC_64          6       //64 times
#define SPL06_TMP_PRC_128         7       //128 times

//SPL06_MEAS_CFG
#define SPL06_MEAS_COEF_RDY       0x80
#define SPL06_MEAS_SENSOR_RDY     0x40        //传感器初始化完成
#define SPL06_MEAS_TMP_RDY        0x20        //有新的温度数据
#define SPL06_MEAS_PRS_RDY        0x10        //有新的气压数据

#define SPL06_MEAS_CTRL_Standby               0x00    //空闲模式
#define SPL06_MEAS_CTRL_PressMeasure          0x01    //单次气压测量
#define SPL06_MEAS_CTRL_TempMeasure           0x02    //单次温度测量
#define SPL06_MEAS_CTRL_ContinuousPress       0x05    //连续气压测量
#define SPL06_MEAS_CTRL_ContinuousTemp        0x06    //连续温度测量
#define SPL06_MEAS_CTRL_ContinuousPressTemp   0x07    //连续气压温度测量

//FIFO_STS
#define SPL06_FIFO_FULL     0x02
#define SPL06_FIFO_EMPTY    0x01

//INT_STS
#define SPL06_INT_FIFO_FULL     0x04
#define SPL06_INT_TMP           0x02
#define SPL06_INT_PRS           0x01

#define SPL06_CFG_T_SHIFT   0x08    //oversampling times>8时必须使用
#define SPL06_CFG_P_SHIFT   0x04

#define SPL06_READ_MASK 0x80
//CFG_REG
#define SPL06_PSR_B2     0x00        //气压值
#define SPL06_PSR_B1     0x01
#define SPL06_PSR_B0     0x02
#define SPL06_TMP_B2     0x03        //温度值
#define SPL06_TMP_B1     0x04
#define SPL06_TMP_B0     0x05

#define SPL06_PSR_CFG    0x06        //气压测量配置
#define SPL06_TMP_CFG    0x07        //温度测量配置
#define SPL06_MEAS_CFG   0x08        //测量模式配置

#define SPL06_CFG_REG    0x09
#define SPL06_INT_STS    0x0A
#define SPL06_FIFO_STS   0x0B

#define SPL06_RESET      0x0C
#define SPL06_ID         0x0D

#define SPL06_COEF       0x10        //0x10-0x21
//0x22-0x27 Reserved
#define SPL06_COEF_SRCE  0x28

typedef enum {
    SPL06_RESULT_OK = 0x00,
    SPL06_RESULT_FAIL
} SPL06_RESULT_t;

typedef struct
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *NSS_GPIO_Port_v;
    uint16_t NSS_Pin_v;

    uint8_t measurementMode;
    uint8_t fifoEnable;

    uint8_t temperatureRate;
    uint8_t pressureRate;

    uint8_t temperatureOversamplingRate;
    uint8_t pressureOversamplingRate;

}SPL06_CONFIG_t;

typedef struct
{
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *NSS_GPIO_Port_v;
    uint16_t NSS_Pin_v;

    uint8_t measurementMode;
    uint8_t fifoEnable;

    uint8_t temperatureRate;
    uint8_t pressureRate;

    uint8_t temperatureOversamplingRate;
    uint8_t pressureOversamplingRate;

    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;

    float KT;
    float KP;

    float P_raw_sc;
    float P_comp;
    float T_raw_sc;
    float T_comp;

}SPL06_ALLCONFIG_t;

typedef struct {
    float pressure;
    float temperature;
    float altitude;
} SPL06_DATA_t;

SPL06_RESULT_t SPL06_Initialize(SPL06_CONFIG_t *spl06Config);
SPL06_RESULT_t SPL06_Config(SPL06_CONFIG_t *spl06Config);
SPL06_RESULT_t SPL06_Update_FIFO(SPL06_DATA_t *spl06Data);
SPL06_RESULT_t SPL06_Update(SPL06_DATA_t *spl06Data);
SPL06_RESULT_t SPL06_UpdatePsr(SPL06_DATA_t *spl06Data);
SPL06_RESULT_t SPL06_UpdateTemp(SPL06_DATA_t *spl06Data);

#endif //UAV_SPL06_H
