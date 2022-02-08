/**
* @file         si24r1.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,01
* @version	    v1.0
*/

/**
  * ---------------------------- 驱动使用说明 ---------------------------------*
  * 需要用到的引脚：3个SPI,CE，CSN引脚,IRQ中断引脚
  * 驱动过程：1.extern uint8_t NRF24L01_Rx_Buf[]（数据接收缓存区）
  *			extern uint8_t NRF24L01_Tx_Rx_Status;
  *			（产生的中断类型：接收成功NRF24L01_TX_OK，发送成功NRF24L01_RX_OK，
  *			发送失败NRF24L01_MAX_TX‘即达到最大重发次数’）
  *			2.NRF24L01_EXTI_IRQHandler() 放到IRQ硬件的中断回调函数里
  *			3.初始化nrf2401（NRF24L01_Initialize）
  *			4.使能发送模式（NRF24L01_RX_Mode）或接受模式（NRF24L01_TX_Mode）
  *			5.发送模式：NRF24L01_TxPacket
  *			  接受模式：接收到的数据将保存在里NRF24L01_Rx_Buf[]
  *
  * ---------------------------- 基本功能介绍 ---------------------------------*
  * 有126个可选的频道
  * 可实现1对6的通讯，1收6发，每一个通道地址不同但是共用同个地址
  * 通道0可以配置40位自身地址，1~5具有32位高位共用地址（通道1设置）和8位自身地址
  * 最高工作速率2Mpbs
  * 具有内部CRC
  * 具有应答
  * 可低速写入，高速发送
  * 中断（中断时产生低电平）：发送完成并且接收到应答信号
  *												接收到数据
  *												达到最大重发次数
  * 两种模式：Enhanced ShockBurstTM:有应答信号，支持自动重发
  *				ShockBurstTM				 :无应答信号，不支持自动重发
  * 发送数据：数据写入寄存器时，CE位置高至少10us发射
  * 通信频率：2400+RF_CH(0~125Mhz)
  * 使能接收模式时CE位置高
  *
  * 通道0.1认数据接收使能，2~5默认为0，不使能数据接收
  * 发送地址填入时从低位写起、
  *
  * 尚未实现功能：1对6通信
  *	DMA数据
  * ------------------------------------------------------------------------*
  */

#ifndef UAV_NRF24L01_H
#define UAV_NRF24L01_H

#include "stm32f4xx_hal.h"

/* 数据包结构 ----------------------------------------------------------------*/
typedef struct
{
    uint8_t cmd;
    int8_t Pitch;
    int8_t Roll;
    int16_t Yaw;
    uint8_t Speed;
    float P;
    float I;
    float D;
}NRF24L01_PACKET_t;

/* 外部调用（勿改动）---------------------------------------------------------*/
#define NRF24L01_MAX_TX                0x10  //达到最大发送次数中断
#define NRF24L01_TX_OK                0x20  //TX发送完成中断
#define NRF24L01_RX_OK                0x40  //接收到数据中断

/* 指令集 --------------------------------------------------------------------*/
/* NRF24L01寄存器操作命令 */
#define NRF24L01_READ_REG_MASK    0x00  //读配置寄存器,低5位为寄存器地址
#define NRF24L01_WRITE_REG_MASK   0x20  //写配置寄存器,低5位为寄存器地址（加上寄存器地址成为最终的写入地址）
#define NRF24L01_RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define NRF24L01_WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define NRF24L01_FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define NRF24L01_FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define NRF24L01_REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NRF24L01_NOP             0xFF  //空操作,可以用来读状态寄存器
/* SPI(NRF24L01)寄存器地址 */
#define NRF24L01_CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
//bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define NRF24L01_EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define NRF24L01_EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define NRF24L01_SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define NRF24L01_SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define NRF24L01_RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define NRF24L01_RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define NRF24L01_STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
//bit5:数据发送完成中断;bit6:接收数据中断
#define NRF24L01_OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define NRF24L01_CD              0x09  //载波检测寄存器,bit0,载波检测;
#define NRF24L01_RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define NRF24L01_RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define NRF24L01_RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define NRF24L01_TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define NRF24L01_RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF24L01_FIFO_STATUS 0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
//bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环

/* 内部定量（根据需求修改） --------------------------------------------------*/
/* 24L01发送接收数据宽度定义 */

typedef enum {
    NRF24L01_RESULT_OK = 0x01,
    NRF24L01_RESULT_FAIL
} NRF24L01_RESULT_t;

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *CSN_GPIO_Port_v;
    uint16_t CSN_Pin_v;
    GPIO_TypeDef *CE_GPIO_Port_v;
    uint16_t CE_Pin_v;
    GPIO_TypeDef *IRQ_GPIO_Port_v;
    uint16_t IRQ_Pin_v;

    uint8_t RX_Address[5];
    uint8_t RX_ADR_Width;
    uint8_t RX_Payload_Width;//用户数据宽度（最大32）
    uint8_t TX_Address[5];
    uint8_t TX_ADR_Width;
    uint8_t TX_Payload_Width;//用户数据宽度（最大32）

} NRF24L01_CONFIG_t;

NRF24L01_RESULT_t NRF24L01_Initialize(NRF24L01_CONFIG_t *nrf24L01Config);                        //初始化
NRF24L01_RESULT_t NRF24L01_Check(NRF24L01_CONFIG_t *nrf24L01Config);                        //检查24L01是否存在
NRF24L01_RESULT_t NRF24L01_RX_Mode(NRF24L01_CONFIG_t *nrf24L01Config);                //配置为接收模式
NRF24L01_RESULT_t NRF24L01_TX_Mode(NRF24L01_CONFIG_t *nrf24L01Config);                //配置为发送模式
NRF24L01_RESULT_t NRF24L01_TxPacket(NRF24L01_CONFIG_t *nrf24L01Config,NRF24L01_PACKET_t * txBuf);        //发送一个包的数据
void NRF24L01_EXTI_IRQHandler(NRF24L01_CONFIG_t *nrf24L01Config);

HAL_StatusTypeDef NRF24L01_Write_Cmd(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t Cmd );
HAL_StatusTypeDef NRF24L01_Write_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t value);
HAL_StatusTypeDef NRF24L01_Read_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *value);
HAL_StatusTypeDef NRF24L01_Write_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len);
HAL_StatusTypeDef NRF24L01_Read_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len);
void delay_us(uint32_t us);

#endif //UAV_NRF24L01_H
