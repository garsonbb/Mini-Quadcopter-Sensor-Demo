/**
* @file         moto.cpp.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,01
* @version	    v1.0
*/
#include "moto.h"
#include "retarget.h"

#define __clamp(__speed__)      \
(__speed__ < 0)     ? (0)      :\
(__speed__ > 500)   ? (500)    :\
__speed__                       \

void MOTO_Initialize(MOTO_CONFIG_t * motoConfig)
{
    motoConfig->LB.speed=0;
    motoConfig->LU.speed=0;
    motoConfig->RB.speed=0;
    motoConfig->RU.speed=0;
    MOTO_SetSpeed(motoConfig);

    HAL_TIM_PWM_Start(motoConfig->LB.htim,motoConfig->LB.channel);
    HAL_TIM_PWM_Start(motoConfig->LU.htim,motoConfig->LU.channel);
    HAL_TIM_PWM_Start(motoConfig->RB.htim,motoConfig->RB.channel);
    HAL_TIM_PWM_Start(motoConfig->RU.htim,motoConfig->RU.channel);
}

void MOTO_SetSpeed(MOTO_CONFIG_t* motoConfig)
{
    __HAL_TIM_SET_COMPARE(motoConfig->LB.htim,motoConfig->LB.channel,__clamp(motoConfig->LB.speed));
    __HAL_TIM_SET_COMPARE(motoConfig->LU.htim,motoConfig->LU.channel,__clamp(motoConfig->LU.speed));
    __HAL_TIM_SET_COMPARE(motoConfig->RB.htim,motoConfig->RB.channel,__clamp(motoConfig->RB.speed));
    __HAL_TIM_SET_COMPARE(motoConfig->RU.htim,motoConfig->RU.channel,__clamp(motoConfig->RU.speed));
    printf("%ld  %ld\r\n",motoConfig->LU.speed,motoConfig->RU.speed);
    printf("%ld  %ld\r\n",motoConfig->LB.speed,motoConfig->RB.speed);
    printf("\r\n");
}

void MOTO_Test(MOTO_CONFIG_t* motoConfig)
{
    motoConfig->LU.speed = 30;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);

    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 30;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);

    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 30;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);

    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 30;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);

    motoConfig->LU.speed = 50;
    motoConfig->LB.speed = 50;
    motoConfig->RU.speed = 50;
    motoConfig->RB.speed = 50;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
    motoConfig->LU.speed = 0;
    motoConfig->LB.speed = 0;
    motoConfig->RU.speed = 0;
    motoConfig->RB.speed = 0;
    MOTO_SetSpeed(motoConfig);
    HAL_Delay(1000);
}