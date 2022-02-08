/**
* @file         moto.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,01
* @version	    v1.0
*/
#ifndef UAV_MOTO_H
#define UAV_MOTO_H

#include "stm32f4xx_hal.h"

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    int32_t speed;
}MOTO_PWM_t;

typedef struct
{
    MOTO_PWM_t LU;//left up
    MOTO_PWM_t LB;//left bottom
    MOTO_PWM_t RU;//right up
    MOTO_PWM_t RB;//right down
}MOTO_CONFIG_t;

void MOTO_Initialize(MOTO_CONFIG_t* motoConfig);
void MOTO_SetSpeed(MOTO_CONFIG_t* motoConfig);
void MOTO_Test(MOTO_CONFIG_t* motoConfig);
#endif //UAV_MOTO_H
