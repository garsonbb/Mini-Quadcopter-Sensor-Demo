/**
* @file         pid.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#ifndef UAV_PID_H
#define UAV_PID_H

#include "stm32f4xx_hal.h"

typedef struct
{
    float Bias;
    float iError;
    float dError;
    float lastBias;
    int iEnableThreshold;
    float iULimit;
    float iDLimit;
    float kP;
    float kI;
    float kD;
    float output;
}PIDMODEL_t;

void PID_Cal(PIDMODEL_t *pidmodel, float target, float measure, int accelerator);

#endif //UAV_PID_H
