/**
* @file         battery.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,03
* @version	    v1.0
*/
#include "battery.h"

float BATTERY_GetValue(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 50);
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(hadc), HAL_ADC_STATE_REG_EOC)){
        return 6.6f * HAL_ADC_GetValue(hadc)/4095.f;
    }
}
