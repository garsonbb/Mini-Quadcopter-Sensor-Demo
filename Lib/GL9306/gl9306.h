/**
* @file         gl9306.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#ifndef UAV_GL9306_H
#define UAV_GL9306_H

#include "stm32f4xx_hal.h"

typedef enum {
    GL9306_RESULT_OK = 0x01,
    GL9306_RESULT_FAIL,
    GL9306_RESULT_NOUPDATE
} GL9306_RESULT_t;

typedef struct {
    UART_HandleTypeDef *huart;
} GL9306_CONFIG_t;

typedef struct {
    int16_t DeltaX;
    int16_t DeltaY;
} GL9306_DATA_t;

GL9306_RESULT_t GL9306_Initialize(GL9306_CONFIG_t *gl9306Config);

void GL9306_IRQ(GL9306_CONFIG_t *gl9306Config);

GL9306_RESULT_t GL9306_Get(GL9306_DATA_t *gl9306Data);

#endif //UAV_GL9306_H
