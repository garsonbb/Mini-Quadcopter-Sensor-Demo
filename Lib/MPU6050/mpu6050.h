/**
* @file         mpu6050.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,11
* @version	    v1.0
*/
#ifndef UAV_MPU6050_H
#define UAV_MPU6050_H

#include "stm32f4xx_hal.h"

#define MPU6050_DEVADDR         0xD0

#define MPU6050_SELF_TESTX		0X0D	//自检寄存器X
#define MPU6050_SELF_TESTY		0X0E	//自检寄存器Y
#define MPU6050_SELF_TESTZ		0X0F	//自检寄存器Z
#define MPU6050_SELF_TESTA		0X10	//自检寄存器A
#define MPU6050_SAMPLE_RATE		0X19	//采样频率分频器
#define MPU6050_CFG				0X1A	//配置寄存器
#define MPU6050_GYRO_CFG		0X1B	//陀螺仪配置寄存器
#define MPU6050_ACCEL_CFG		0X1C	//加速度计配置寄存器
#define MPU6050_MOTION_DET		0X1F	//运动检测阀值设置寄存器
#define MPU6050_FIFO_EN			0X23	//FIFO使能寄存器
#define MPU6050_I2CMST_CTRL		0X24	//IIC主机控制寄存器
#define MPU6050_I2CSLV0_ADDR	0X25	//IIC从机0器件地址寄存器
#define MPU6050_I2CSLV0			0X26	//IIC从机0数据地址寄存器
#define MPU6050_I2CSLV0_CTRL	0X27	//IIC从机0控制寄存器
#define MPU6050_I2CSLV1_ADDR	0X28	//IIC从机1器件地址寄存器
#define MPU6050_I2CSLV1			0X29	//IIC从机1数据地址寄存器
#define MPU6050_I2CSLV1_CTRL	0X2A	//IIC从机1控制寄存器
#define MPU6050_I2CSLV2_ADDR	0X2B	//IIC从机2器件地址寄存器
#define MPU6050_I2CSLV2			0X2C	//IIC从机2数据地址寄存器
#define MPU6050_I2CSLV2_CTRL	0X2D	//IIC从机2控制寄存器
#define MPU6050_I2CSLV3_ADDR	0X2E	//IIC从机3器件地址寄存器
#define MPU6050_I2CSLV3			0X2F	//IIC从机3数据地址寄存器
#define MPU6050_I2CSLV3_CTRL	0X30	//IIC从机3控制寄存器
#define MPU6050_I2CSLV4_ADDR	0X31	//IIC从机4器件地址寄存器
#define MPU6050_I2CSLV4			0X32	//IIC从机4数据地址寄存器
#define MPU6050_I2CSLV4_DO		0X33	//IIC从机4写数据寄存器
#define MPU6050_I2CSLV4_CTRL	0X34	//IIC从机4控制寄存器
#define MPU6050_I2CSLV4_DI		0X35	//IIC从机4读数据寄存器

#define MPU6050_I2CMST_STA		0X36	//IIC主机状态寄存器
#define MPU6050_INTBP_CFG		0X37	//中断/旁路设置寄存器
#define MPU6050_INT_EN			0X38	//中断使能寄存器
#define MPU6050_INT_STA			0X3A	//中断状态寄存器

#define MPU6050_ACCEL_XOUT_H	0X3B	//加速度值,X轴高8位寄存器
#define MPU6050_ACCEL_XOUT_L	0X3C	//加速度值,X轴低8位寄存器
#define MPU6050_ACCEL_YOUT_H	0X3D	//加速度值,Y轴高8位寄存器
#define MPU6050_ACCEL_YOUT_L	0X3E	//加速度值,Y轴低8位寄存器
#define MPU6050_ACCEL_ZOUT_H	0X3F	//加速度值,Z轴高8位寄存器
#define MPU6050_ACCEL_ZOUT_L	0X40	//加速度值,Z轴低8位寄存器

#define MPU6050_TEMP_OUT_H		0X41	//温度值高八位寄存器
#define MPU6050_TEMP_OUT_L		0X42	//温度值低8位寄存器

#define MPU6050_GYRO_XOUT_H		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU6050_GYRO_XOUT_L		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU6050_GYRO_YOUT_H		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU6050_GYRO_YOUT_L		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU6050_GYRO_ZOUT_H		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU6050_GYRO_ZOUT_L		0X48	//陀螺仪值,Z轴低8位寄存器

#define MPU6050_I2CSLV0_DO		0X63	//IIC从机0数据寄存器
#define MPU6050_I2CSLV1_DO		0X64	//IIC从机1数据寄存器
#define MPU6050_I2CSLV2_DO		0X65	//IIC从机2数据寄存器
#define MPU6050_I2CSLV3_DO		0X66	//IIC从机3数据寄存器

#define MPU6050_I2CMST_DELAY	0X67	//IIC主机延时管理寄存器
#define MPU6050_SIGPATH_RST		0X68	//信号通道复位寄存器
#define MPU6050_MDETECT_CTRL	0X69	//运动检测控制寄存器
#define MPU6050_USER_CTRL		0X6A	//用户控制寄存器
#define MPU6050_PWR_MGMT_1		0X6B	//电源管理寄存器1
#define MPU6050_PWR_MGMT_2		0X6C	//电源管理寄存器2 
#define MPU6050_FIFO_CNT_H		0X72	//FIFO计数寄存器高八位
#define MPU6050_FIFO_CNT_L		0X73	//FIFO计数寄存器低八位
#define MPU6050_FIFO_RW			0X74	//FIFO读写寄存器
#define MPU6050_WHO_AM_I		0X75	//器件ID寄存器

#define CALIBRATE_TIMES 1000

typedef enum {
    MPU6050_RESULT_OK = 0x01,
    MPU6050_RESULT_FAIL
} MPU6050_RESULT_t;

typedef enum {
    ACCEL_SCALE_2G  = 0x00,
    ACCEL_SCALE_4G  = 0x08,
    ACCEL_SCALE_8G  = 0x10,
    ACCEL_SCALE_16G = 0x18
} MPU6050_ACCEL_SCALE_t;

typedef enum {
    GYRO_SCALE_250dps  = 0x00,
    GYRO_SCALE_500dps  = 0x08,
    GYRO_SCALE_1000dps = 0x10,
    GYRO_SCALE_2000dps = 0x18
} MPU6050_GYRO_SCALE_t;

typedef struct
{
    I2C_HandleTypeDef *hi2c;

    MPU6050_ACCEL_SCALE_t ACCEL_SCALE;
    MPU6050_GYRO_SCALE_t GYRO_SCALE;

    float ARES;//Accel resolution
    float GRES;//Gyro resolution

    float ACCEL_OFFSET[3];
    float GYRO_OFFSET[3];
}MPU6050_ALLCONFIG_t;

typedef struct
{
    I2C_HandleTypeDef *hi2c;

    MPU6050_ACCEL_SCALE_t ACCEL_SCALE;
    MPU6050_GYRO_SCALE_t GYRO_SCALE;
}MPU6050_CONFIG_t;

typedef struct {
    float accel[3];//x,y,z  unit:g
    uint16_t accel_raw[3];
    float gyro[3];//x,y,z   unit:rad/s
    uint16_t gyro_raw[3];
    float temperature;
    uint16_t temperature_raw;
} MPU6050_DATA_t;

MPU6050_RESULT_t MPU6050_Initialize(MPU6050_CONFIG_t *MPU6050_CONFIG);
MPU6050_RESULT_t MPU6050_Config(MPU6050_CONFIG_t *MPU6050_CONFIG);
MPU6050_RESULT_t MPU6050_Calibrate();
MPU6050_RESULT_t MPU6050_Get7DOF(MPU6050_DATA_t *MPU6050_DATA);



#endif //UAV_MPU6050_H
