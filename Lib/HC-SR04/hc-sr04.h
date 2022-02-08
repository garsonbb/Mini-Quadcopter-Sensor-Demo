/**
* @file         hc-sr04.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#ifndef UAV_HC_SR04_H
#define UAV_HC_SR04_H

#include "stm32f4xx_hal.h"

typedef struct
{
    TIM_HandleTypeDef *triggerTim;
    uint32_t triggerTimChannel;
    TIM_HandleTypeDef *echoTim;
    uint32_t echoTimChannel;
}HCSR04_CONFIG_t;

void HCSR04_Initialize(HCSR04_CONFIG_t *hcsr04Config);
void HCSR04_IRQ(HCSR04_CONFIG_t *hcsr04Config);
float HCSR04_GetDistance(float temperature);

#endif //UAV_HC_SR04_H
