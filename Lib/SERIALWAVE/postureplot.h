/**
* @file         postureplot.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#ifndef UAV_POSTUREPLOT_H
#define UAV_POSTUREPLOT_H

#include "stm32f4xx_hal.h"

void plotTest(UART_HandleTypeDef *huart);
void posturePlot(UART_HandleTypeDef *huart);

#endif //UAV_POSTUREPLOT_H
