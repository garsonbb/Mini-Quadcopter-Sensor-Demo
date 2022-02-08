/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "retarget.h"
#include "air530z.h"
#include "spl06.h"
#include "hc-sr04.h"
#include "nrf24l01.h"
#include "mpu6050.h"
#include "gl9306.h"
#include "imu.h"
#include "postureplot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* MPU9250 */
MPU6050_CONFIG_t mpu6050Config;
MPU6050_DATA_t mpu6050Data;

/* AIR530Z */
AIR530Z_CONFIG_t air530ZConfig;
AIR530Z_GGA_t air530ZGga;
AIR530Z_RMC_t air530ZRmc;

/* SPL06 */
SPL06_CONFIG_t spl06Config;
SPL06_DATA_t spl06Data;

/* HC-SR04 */
HCSR04_CONFIG_t hcsr04Config;

/* NRF24L01 */
NRF24L01_CONFIG_t nrf24L01Config;
uint8_t RxAddr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};
uint8_t TxAddr[5] = {0x34, 0x43, 0x10, 0x10, 0x02};
extern uint8_t NRF24L01_Tx_Rx_Status;
extern NRF24L01_PACKET_t NRF24L01_Rx_Buf;

/* IMU */
extern IMU_DATA_t imuData;

/* GL9306 */
GL9306_CONFIG_t gl9306Config;
GL9306_DATA_t gl9306Data;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void TEST_AIR530Z(void);
void TEST_SPL06(void);
void TEST_HCSR04(void);
void TEST_NRF24(void);
void TEST_MPU6050(void);
void TEST_IMU(void);
void TEST_GL9306(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  uint32_t baseTime = 0;
  uint8_t eventIMU = 0;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  RetargetInit(&huart6);
  printf("start\r\n");

    /* MPU6050 */
    mpu6050Config.hi2c = &hi2c1;
    mpu6050Config.ACCEL_SCALE = ACCEL_SCALE_2G;
    mpu6050Config.GYRO_SCALE = GYRO_SCALE_250dps;
    MPU6050_Initialize(&mpu6050Config);
    HAL_Delay(500);
    MPU6050_Config(&mpu6050Config);
    HAL_Delay(500);
    MPU6050_Calibrate(&mpu6050Config);
    HAL_Delay(500);



    /* SPL06 */
    spl06Config.hi2c = &hi2c1;
    spl06Config.fifoEnable = 0;
    spl06Config.measurementMode = SPL06_MEAS_CTRL_ContinuousPressTemp;
    spl06Config.pressureRate = SPL06_PM_RATE_128;
    spl06Config.pressureOversamplingRate = SPL06_PM_PRC_16;
    spl06Config.temperatureRate = SPL06_TMP_RATE_32;
    spl06Config.temperatureOversamplingRate = SPL06_TMP_PRC_16;
//    SPL06_Initialize(&spl06Config);
//    SPL06_Config(&spl06Config);

    /* AIR530Z */
    air530ZConfig.huart = &huart1;
    AIR530Z_Initialize(&air530ZConfig);
    AIR530Z_Config(&air530ZConfig,AIR530Z_CFG_BAUDRATE_115200);
    AIR530Z_Config(&air530ZConfig,AIR530Z_CFG_SYSTEM_GPS_BDS_GLONASS);
    AIR530Z_Config(&air530ZConfig,AIR530Z_CFG_NMEA_ONLY_GGA_RMC);

    /* HC-SR04 */
    hcsr04Config.triggerTim = &htim2;
    hcsr04Config.triggerTimChannel=TIM_CHANNEL_1;
    hcsr04Config.echoTim = &htim2;
    hcsr04Config.echoTimChannel = TIM_CHANNEL_2;
    HCSR04_Initialize(&hcsr04Config);

    /* NRF24L01 */
    nrf24L01Config.hspi = &hspi2;
    nrf24L01Config.CSN_GPIO_Port_v = NRF_CSN_GPIO_Port;
    nrf24L01Config.CSN_Pin_v = NRF_CSN_Pin;
    nrf24L01Config.CE_GPIO_Port_v = NRF_CE_GPIO_Port;
    nrf24L01Config.CE_Pin_v = NRF_CE_Pin;
    nrf24L01Config.IRQ_GPIO_Port_v = NRF_IRQ_GPIO_Port;
    nrf24L01Config.IRQ_Pin_v = NRF_IRQ_Pin;
    nrf24L01Config.RX_Payload_Width = 20;
    nrf24L01Config.RX_ADR_Width = 5;
    nrf24L01Config.TX_Payload_Width = 20;
    nrf24L01Config.TX_ADR_Width = 5;
    for (int i = 0; i < 5; ++i) {
        nrf24L01Config.TX_Address[i] = TxAddr[i];
        nrf24L01Config.RX_Address[i] = RxAddr[i];
    }
//    NRF24L01_Initialize(&nrf24L01Config);
//    NRF24L01_RX_Mode(&nrf24L01Config);

    /* GL9306 */
    gl9306Config.huart = &huart2;
    GL9306_Initialize(&gl9306Config);

    /* IMU */
//    HAL_TIM_Base_Start_IT(&htim4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);

    baseTime = TIM4->CNT;
    eventIMU = baseTime % 40;
//    if(eventIMU == 0)
//    {
//        MPU6050_Get7DOF(&mpu6050Data);
//        IMU_UPDATE(mpu6050Data.gyro[0],mpu6050Data.gyro[1],mpu6050Data.gyro[2],mpu6050Data.accel[0],mpu6050Data.accel[1],mpu6050Data.accel[2]);
//    }
//
//      TEST_IMU();
//      HAL_Delay(400);
//      TEST_SPL06();
//      TEST_AIR530Z();
//      TEST_GL9306();
        TEST_HCSR04();
  }
#pragma clang diagnostic pop
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/*** Test function begin ***/
void TEST_AIR530Z()
{
    AIR530Z_RESULT_t res=0;
    res=AIR530Z_GetLocation(&air530ZGga,&air530ZRmc);
    if(res==AIR530Z_RESULT_OK){
        printf("%s: %c:%f , %c:%f .\r\n",air530ZRmc.RMC,air530ZRmc.uLat,air530ZRmc.lat,air530ZRmc.uLon,air530ZRmc.lon);
        printf("%s: altitude:%f .\r\n\r\n",air530ZGga.GGA,air530ZGga.msl);
        HAL_Delay(1000);
    }else if (res == AIR530Z_RESULT_LOSTCONN)
    {
        printf("Lost Connction.\r\n\r\n");
    } else if (res == AIR530Z_RESULT_NOUPDATE){
        printf("NOUPDATE\r\n\r\n");
        HAL_Delay(1000);
    }
}

void TEST_SPL06()
{
    HAL_Delay(100);
//    SPL06_Update_FIFO(&spl06Data);
    SPL06_Update(&spl06Data);
    printf("temperature: %.2fC,pressure: %.2fPa,altitude: %.2fmm\r\n",spl06Data.temperature,spl06Data.pressure,spl06Data.altitude*100);
}

void TEST_HCSR04()
{
    printf("Distance: %.2f\r\n",HCSR04_GetDistance(27));
    HAL_Delay(200);
}

void TEST_NRF24() {
    static uint16_t timesOut;

    if (NRF24L01_Tx_Rx_Status == NRF24L01_RX_OK) {
        NRF24L01_Tx_Rx_Status = 0;
        timesOut = 0;
//        PROTECT_LOCK &= 1;
//
//        rxPitch = NRF24L01_Rx_Buf.Pitch;
//        rxRoll = NRF24L01_Rx_Buf.Roll;
//        rxYaw = NRF24L01_Rx_Buf.Yaw;
//        rxSpeed = NRF24L01_Rx_Buf.Speed;
//        rxP = NRF24L01_Rx_Buf.P;
//        rxI = NRF24L01_Rx_Buf.I;
//        rxD = NRF24L01_Rx_Buf.D;

//        PID_PITCH_ANGLE.kP = rxP;
//        PID_ROLL_ANGLE.kP = rxP;
//        printf("rxPitch: %d ,rxRoll: %d ,rxYaw: %d ,rxSpeed: %d ,rxP: %f ,rxI: %f ,rxD: %f .\r\n", rxPitch, rxRoll,
//               rxYaw, rxSpeed, rxP, rxI, rxD);
        printf("rxPitch: %d ,rxRoll: %d ,rxYaw: %d ,rxSpeed: %d .\r\n", NRF24L01_Rx_Buf.Pitch,  NRF24L01_Rx_Buf.Roll, NRF24L01_Rx_Buf.Yaw,  NRF24L01_Rx_Buf.Speed);
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    } else {
        //printf("Connect NO:: 0x%02x\r\n",NRF24L01_Tx_Rx_Status);
        timesOut++;
    }
    if (timesOut > 100)//lock after 5 times lost connection, just for test.
    {
//        PROTECT_LOCK |= 1<<1;
    }
}

void TEST_MPU6050() {

    // uncomment when TIM5 interrupt is closed.
    MPU6050_Get7DOF(&mpu6050Data);
//    printf("x:%d y:%d z:%d\r\n", mpu6050Data.accel_raw[0], mpu6050Data.accel_raw[1], mpu6050Data.accel_raw[2]);
    printf("x:%f y:%f z:%f\r\n", mpu6050Data.accel[0], mpu6050Data.accel[1], mpu6050Data.accel[2]);
//    uint8_t Rec_Data[2];
//    int16_t temp;

    // Read 2 BYTES of data starting from TEMP_OUT_H_REG register

//    HAL_I2C_Mem_Read(&hi2c1, MPU6050_DEVADDR, MPU6050_TEMP_OUT_H, 1, Rec_Data, 2, 100);
//
//    temp = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
//    printf("%f\r\n", (float)((int16_t)temp / (float)340.0 + (float)36.53));
    printf("temp:%f\r\n", mpu6050Data.temperature);
    /*
    // print as word
    printf("x:%f y:%f z:%f\r\n", mpu9250Data.accel[0], mpu9250Data.accel[1], mpu9250Data.accel[2]);
    printf("temp:%f\r\n", mpu9250Data.temperature);
    printf("roll: %f ,pitch: %f ,yaw: %f .\r\n", roll, pitch, yaw);
    */
    // plot in Serial Tool
    // posturePlot(&huart6);
}
void TEST_IMU() {
    HAL_Delay(2);
    posturePlot(&huart6);
//    printf("pitch: %.2f,roll: %.2f,yaw: %.2f.\r\n",imuData.pitch,imuData.roll,imuData.yaw);
}

void TEST_GL9306() {
    HAL_Delay(40);
    GL9306_Get(&gl9306Data);
    printf("DeltaX: %d, DeltaY: %d.\r\n",gl9306Data.DeltaX,gl9306Data.DeltaY);
}

void TEST_IMU_TICK()
{
    uint32_t tick_s = 0;
    uint32_t tick_e = 0;
    tick_s = HAL_GetTick();
    for (int t = 0; t < 3000; ++t) {
        MPU6050_Get7DOF(&mpu6050Data);
        IMU_UPDATE(mpu6050Data.gyro[0],mpu6050Data.gyro[1],mpu6050Data.gyro[2],mpu6050Data.accel[0],mpu6050Data.accel[1],mpu6050Data.accel[2]);
    }
    tick_e = HAL_GetTick();
    printf("%lu\r\n",tick_e-tick_s);
}

/*** Test function end ***/

/*** IRQ function begin ***/

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == air530ZConfig.huart->Instance)
    {
        AIR530Z_IRQ(&air530ZConfig);
    }
    if(huart->Instance == gl9306Config.huart->Instance)
    {
        GL9306_IRQ(&gl9306Config);
    }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim==hcsr04Config.echoTim)
    {
        HCSR04_IRQ(&hcsr04Config);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == NRF_IRQ_Pin) {
        NRF24L01_EXTI_IRQHandler(&nrf24L01Config);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//    if (htim == &htim4){
//        MPU6050_Get7DOF(&mpu6050Data);
//        IMU_UPDATE(mpu6050Data.gyro[0],mpu6050Data.gyro[1],mpu6050Data.gyro[2],mpu6050Data.accel[0],mpu6050Data.accel[1],mpu6050Data.accel[2]);
//    }
}

/*** IRQ function end ***/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
