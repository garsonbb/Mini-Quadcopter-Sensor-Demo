/**
* @file         controller.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#ifndef UAV_CONTROLLER_H
#define UAV_CONTROLLER_H

#include "stm32f4xx_hal.h"
#include "imu.h"

void Controller_Init(void);
void Controller_Moto(uint16_t baseSpeed,int16_t targetRoll,int16_t targetPitch,int16_t targetYaw, IMU_DATA_t *measure);

#endif //UAV_CONTROLLER_H
