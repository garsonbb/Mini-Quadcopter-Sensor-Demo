/**
* @file         postureplot.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#include "postureplot.h"

#include <math.h>
#include "serialwave.h"
#include "imu.h"

extern IMU_DATA_t imuData;

// 串口示波器测试函数
void plotTest(UART_HandleTypeDef *huart)
{
    char buffer[100], len; // 缓冲区最大需要83bytes
    static float key = 0.0f;
    static ws_timestamp_t ts = { // 时间戳
            17, 11, 20,      // year, month, day
            12, 30, 45,      // hour, minute, second
            120, 50          // msec, sample rate
    };

    // 每秒钟发送一次时间戳
    if ((int64_t)(key * 50.0) % 100 == 0) {
        len = ws_send_timestamp(buffer, &ts);
        HAL_UART_Transmit(huart,buffer,len,0xffff);// 串口发送数据
    }

    // 点发送模式 int8类型（8位有符号整形）
    len = ws_point_int8(buffer, CH1, (int8_t)(sinf(key) * 64));
    HAL_UART_Transmit(huart,buffer,len,0xffff);// 串口发送数据

    // 点发送模式 int16类型（16位有符号整形）
    len = ws_point_int16(buffer, CH2, (int16_t)(sinf(key) * 4096));
    HAL_UART_Transmit(huart,buffer,len,0xffff);// 串口发送数据

    // 点发送模式 int32类型（32位有符号整形）
    len = ws_point_int32(buffer, CH3, (int32_t)(sinf(key) * 2048));
    HAL_UART_Transmit(huart,buffer,len,0xffff);// 串口发送数据

    // 点发送模式 float类型（单精度浮点型）
    len = ws_point_float(buffer, CH4, (float)(sinf(key) * 512));
    HAL_UART_Transmit(huart,buffer,len,0xffff);// 串口发送数据

    // 同步模式(帧发送模式)
    ws_frame_init(buffer);
    ws_add_int8(buffer, CH5, (int8_t)(sinf(key) * 128));
    ws_add_int16(buffer, CH6, (int16_t)(sinf(key) * 700));
    ws_add_int32(buffer, CH7, (int32_t)(sinf(key) * 1024));
    ws_add_float(buffer, CH9, (float)(sinf(key) * 256));
    HAL_UART_Transmit(huart,buffer,ws_frame_length(buffer),0xffff);// 串口发送数据

    // 更新时间计数
    key += 0.02f;
}

void posturePlot(UART_HandleTypeDef *huart)
{
    char buffer[100], len; // 缓冲区最大需要83bytes

    // 同步模式(帧发送模式)
    ws_frame_init(buffer);
    ws_add_float(buffer, CH1, (float)(imuData.pitch));
    ws_add_float(buffer, CH2, (float)(imuData.roll));
    ws_add_float(buffer, CH3, (float)(imuData.yaw));
    HAL_UART_Transmit(huart,buffer,ws_frame_length(buffer),0xffff);// 串口发送数据
}