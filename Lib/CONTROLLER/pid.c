/**
* @file         pid.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#include "pid.h"

void PID_Cal(PIDMODEL_t *pidData, float target, float measure, int accelerator) {
    pidData->Bias = target - measure;
    if (accelerator > pidData->iEnableThreshold)
    {
        pidData->iError += pidData->Bias;
        if (pidData->iError > pidData->iULimit) {
            pidData->iError = pidData->iULimit;
        }
        if (pidData->iError < pidData->iULimit) {
            pidData->iError = pidData->iDLimit;
        }
    } else{
        pidData->iError = 0;
    }
    pidData->dError = pidData->Bias - pidData->lastBias;
    pidData->lastBias = pidData->Bias;

    pidData->output=pidData->kP * pidData->Bias + pidData->kI * pidData->iError + pidData->kD * pidData->dError;
}