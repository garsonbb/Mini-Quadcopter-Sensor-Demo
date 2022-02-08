/**
* @file         hc-sr04.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#include "hc-sr04.h"

uint8_t FLAG_ECHOING = 0;

uint32_t TICK_START=0;
uint32_t TICK_END=0;
uint32_t TICK_LAST=0;

float HALF_SPEED=170.f;

float Distance = 0;

void HCSR04_Initialize(HCSR04_CONFIG_t *hcsr04Config)
{
    HAL_TIM_PWM_Start(hcsr04Config->triggerTim,hcsr04Config->triggerTimChannel);
    HAL_TIM_IC_Start_IT(hcsr04Config->echoTim,hcsr04Config->echoTimChannel);
}

void HCSR04_IRQ(HCSR04_CONFIG_t *hcsr04Config)
{ //每200ms执行一次
    if( (hcsr04Config->echoTim->Instance->CCER & TIM_CCER_CC2P_Msk) == TIM_INPUTCHANNELPOLARITY_RISING )
    {
        TICK_START=HAL_TIM_ReadCapturedValue(hcsr04Config->echoTim,hcsr04Config->echoTimChannel);//TICK_START=hcsr04Config->echoTim->Instance->CCR2;//存储高电平开始的时刻
        __HAL_TIM_SET_CAPTUREPOLARITY(hcsr04Config->echoTim, TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);//设为下降沿捕捉
        FLAG_ECHOING = 1;
    }
    else
    {
        TICK_END=HAL_TIM_ReadCapturedValue(hcsr04Config->echoTim,hcsr04Config->echoTimChannel);//TICK_END=hcsr04Config->echoTim->Instance->CCR2;//存储高电平结束的时刻
        TICK_LAST=TICK_END-TICK_START;
        TICK_LAST=TICK_LAST*10/3.f;//high_time*(htim->Instance->PSC+1)/84; //(次数/(84M/PSC))*10e6=次数*10/3=次数*280*10e6/84M=次数*10/3
        __HAL_TIM_SET_CAPTUREPOLARITY(hcsr04Config->echoTim, hcsr04Config->echoTimChannel,TIM_INPUTCHANNELPOLARITY_RISING); //设为上升沿捕捉，为下次计算做准备
        FLAG_ECHOING = 0;
    }
}

float HCSR04_GetDistance(float temperature)
{
    HALF_SPEED = (331.45f+0.61f*temperature)/2;
    Distance=TICK_LAST*HALF_SPEED/1000;
    return Distance;
}