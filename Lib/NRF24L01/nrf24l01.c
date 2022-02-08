/**
* @file         nrf24l01.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,01
* @version	    v1.0
*/
#include "nrf24l01.h"
#include "retarget.h"

//HAL_StatusTypeDef NRF24L01_Write_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t value);
//HAL_StatusTypeDef NRF24L01_Read_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *value);
//HAL_StatusTypeDef NRF24L01_Write_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len);
//HAL_StatusTypeDef NRF24L01_Read_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len);
//void delay_us(uint32_t us);

uint8_t NRF24L01_Tx_Rx_Status;
NRF24L01_PACKET_t NRF24L01_Rx_Buf; //NRF24L01 数据接收缓存区
NRF24L01_PACKET_t NRF24L01_Tx_Buf;

NRF24L01_RESULT_t NRF24L01_Initialize(NRF24L01_CONFIG_t *nrf24L01Config)
{
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 0);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);

    while(NRF24L01_Check(nrf24L01Config)==NRF24L01_RESULT_FAIL)
    {
        printf("NRF24L01 Init Fail!\r\n");
        HAL_Delay(300);
    }
    printf("NRF24L01 Init Success!\r\n");

    NRF24L01_Write_Buf(nrf24L01Config,NRF24L01_RX_ADDR_P0, nrf24L01Config->RX_Address, nrf24L01Config->RX_ADR_Width); //设置RX节点地址（自动应答地址）,主要为了使能ACK
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_EN_RXADDR,0x01);//使能通道0的接收地址
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_EN_AA,0x01); //使能通道0自动应答
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_RX_PW_P0,nrf24L01Config->RX_Payload_Width);//选择通道0的有效数据宽度
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_RF_CH,55); //设置RF通道为40hz(1-64Hz都可以)

    NRF24L01_Write_Buf(nrf24L01Config,NRF24L01_TX_ADDR, nrf24L01Config->TX_Address, nrf24L01Config->TX_ADR_Width);
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_SETUP_RETR,0x1a); //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次 0x1A
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_RF_SETUP,0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益关闭 （注意：低噪声增益关闭/开启直接影响通信,要开启都开启，要关闭都关闭0x0f）

    NRF24L01_Write_Cmd(nrf24L01Config,NRF24L01_FLUSH_TX);                // 清除TX_FIFO
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_STATUS,0X7E);                // 清除所有中断,防止一进去接收模式就触发中断
    NRF24L01_RX_Mode(nrf24L01Config); //默认为接收模式
    return NRF24L01_RESULT_OK;
}

NRF24L01_RESULT_t NRF24L01_Check(NRF24L01_CONFIG_t *nrf24L01Config)
{
    uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
    uint8_t i=0;
    NRF24L01_Write_Buf(nrf24L01Config,NRF24L01_TX_ADDR, buf, 5); //写入5个字节的地址.
    NRF24L01_Read_Buf(nrf24L01Config,NRF24L01_TX_ADDR, buf, 5); //读出写入的地址
    for(i = 0; i < 5; i++){if(buf[i] != 0XA5)break;}
    if(i != 5)return NRF24L01_RESULT_FAIL; //检测24L01错误
    return NRF24L01_RESULT_OK;		 //检测到24L01
}




//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了
NRF24L01_RESULT_t NRF24L01_RX_Mode(NRF24L01_CONFIG_t *nrf24L01Config)
{
    HAL_StatusTypeDef result;
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 0);
    result = NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_CONFIG,0x0f);
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 1);//CE为高,进入接收模式

    if(result != HAL_OK){return NRF24L01_RESULT_FAIL;}

    //delay_us(200);
    HAL_Delay(1);
    return NRF24L01_RESULT_OK;
}

//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了
//CE为高大于10us,则启动发送.
NRF24L01_RESULT_t NRF24L01_TX_Mode(NRF24L01_CONFIG_t *nrf24L01Config)
{
    HAL_StatusTypeDef result;
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 0);
    result = NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_CONFIG,0x0e);//CE为高,10us后启动发送
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 1);
    if(result != HAL_OK){return NRF24L01_RESULT_FAIL;}
    return NRF24L01_RESULT_OK;
}


NRF24L01_RESULT_t NRF24L01_TxPacket(NRF24L01_CONFIG_t *nrf24L01Config,NRF24L01_PACKET_t * txBuf)
{
    HAL_StatusTypeDef result;
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 0);
    result = NRF24L01_Write_Buf(nrf24L01Config,NRF24L01_WR_TX_PLOAD, (uint8_t *)txBuf, nrf24L01Config->TX_Payload_Width); //写数据到TX BUF  32个字节
    HAL_GPIO_WritePin(nrf24L01Config->CE_GPIO_Port_v, nrf24L01Config->CE_Pin_v, 1); //启动发送

    if(result != HAL_OK){return NRF24L01_RESULT_FAIL;}

    //delay_us(10);  //CE持续高电平10us
    HAL_Delay(1);
    return NRF24L01_RESULT_OK;
}



HAL_StatusTypeDef NRF24L01_Write_Cmd(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t Cmd ) {
    HAL_StatusTypeDef spiRes;

    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 0);
    spiRes = HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &Cmd, 1,0xffff);
    Cmd = 0xff;
    spiRes |= HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &Cmd, 1,0xffff);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);

    return spiRes;
}

HAL_StatusTypeDef NRF24L01_Write_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t value) {
    HAL_StatusTypeDef spiRes;

    reg=reg|NRF24L01_WRITE_REG_MASK;
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 0);
    spiRes = HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &reg, 1,0xffff);
    spiRes |= HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &value, 1,0xffff);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);

    return spiRes;
}

HAL_StatusTypeDef NRF24L01_Read_Reg(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *value) {
    HAL_StatusTypeDef spiRes;

    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 0);
    spiRes = HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &reg, 1,0xffff);
    spiRes |= HAL_SPI_TransmitReceive(nrf24L01Config->hspi, (uint8_t *) value, (uint8_t *) value, 1,0xffff);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);
    return spiRes;
}

HAL_StatusTypeDef NRF24L01_Write_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len) {
    HAL_StatusTypeDef spiRes;

    reg = reg|NRF24L01_WRITE_REG_MASK;
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 0);
    spiRes = HAL_SPI_Transmit(nrf24L01Config->hspi, (uint8_t *) &reg, 1, 0xffff);
    spiRes |= HAL_SPI_TransmitReceive(nrf24L01Config->hspi, (uint8_t *) transmitBuf, (uint8_t *) transmitBuf, len, 0xffff);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);
    return spiRes;
}

HAL_StatusTypeDef NRF24L01_Read_Buf(NRF24L01_CONFIG_t *nrf24L01Config, uint8_t reg, uint8_t *transmitBuf, uint8_t len) {
    HAL_StatusTypeDef spiRes;

    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 0);
    spiRes = HAL_SPI_TransmitReceive(nrf24L01Config->hspi, (uint8_t *) &reg, (uint8_t *) &reg, 1, 0xffff);
    spiRes |= HAL_SPI_TransmitReceive(nrf24L01Config->hspi, (uint8_t *) transmitBuf, (uint8_t *) transmitBuf, len, 0xffff);
    HAL_GPIO_WritePin(nrf24L01Config->CSN_GPIO_Port_v, nrf24L01Config->CSN_Pin_v, 1);
    return spiRes;
}

void delay_us(uint32_t us)
{
    uint32_t start_val,tick_n,delays,wait;

    start_val = SysTick->VAL;
    tick_n = HAL_GetTick();
    //sysc = 168000;  //SystemCoreClock / (1000U / uwTickFreq);
    delays =us * 84; //sysc / 1000 * udelay;
    if(delays > start_val)
    {
        while(HAL_GetTick() == tick_n){}
        wait = 84000 + start_val - delays;
        while(wait < SysTick->VAL){}
    }
    else
    {
        wait = start_val - delays;
        while(wait < SysTick->VAL && HAL_GetTick() == tick_n){}
    }
}
void NRF24L01_EXTI_IRQHandler(NRF24L01_CONFIG_t *nrf24L01Config) {
    uint8_t status=0;

    NRF24L01_Read_Reg(nrf24L01Config,NRF24L01_STATUS,&status); //读取状态寄存器的值
        if(status & NRF24L01_MAX_TX) //达到最大重发次数
        {
            NRF24L01_Tx_Rx_Status = NRF24L01_MAX_TX;
            NRF24L01_Write_Cmd(nrf24L01Config,NRF24L01_FLUSH_TX); //清除TX FIFO寄存器
            NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_STATUS, status); //清除中断标志
            return;
        }
        else if(status & NRF24L01_TX_OK) //发送完成
        {
            NRF24L01_Tx_Rx_Status = NRF24L01_TX_OK;
            NRF24L01_RX_Mode(nrf24L01Config);
            NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_STATUS, status); //清除中断标志
            return;
        }
        else if(status & NRF24L01_RX_OK) //接收完成
        {
            NRF24L01_Tx_Rx_Status = NRF24L01_RX_OK;
            NRF24L01_Read_Buf(nrf24L01Config,NRF24L01_RD_RX_PLOAD, (uint8_t *)&NRF24L01_Rx_Buf, nrf24L01Config->RX_Payload_Width); //读取数据
            NRF24L01_Write_Cmd(nrf24L01Config,NRF24L01_FLUSH_RX); //清除RX FIFO寄存器
            NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_STATUS, status); //清除中断标志
            return;
        }
    NRF24L01_Write_Reg(nrf24L01Config,NRF24L01_STATUS, 0x7F); //清除中断标志
}



