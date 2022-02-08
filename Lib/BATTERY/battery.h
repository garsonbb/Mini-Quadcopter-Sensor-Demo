/**
* @file         battery.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,03
* @version	    v1.0
*/
#ifndef UAV_BATTERY_H
#define UAV_BATTERY_H

#include "stm32f4xx_hal.h"
float BATTERY_GetValue(ADC_HandleTypeDef *hadc);

#endif //UAV_BATTERY_H
