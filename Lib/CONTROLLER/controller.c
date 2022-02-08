/**
* @file         controller.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#include "controller.h"
#include "pid.h"
#include "moto.h"
#include "retarget.h"

extern MOTO_CONFIG_t motoConfig;

PIDMODEL_t PID_ROLL_ANGLE;
PIDMODEL_t PID_PITCH_ANGLE;
PIDMODEL_t PID_YAW_ANGLE;

uint8_t PROTECT_LOCK = 0; //bit 0: slope lock; bit 1: lost connection lock

void Controller_Init()
{
    PID_ROLL_ANGLE.kP = 2;
    PID_ROLL_ANGLE.kI = 0;
    PID_ROLL_ANGLE.kD = 0;
    PID_ROLL_ANGLE.iULimit = 0;
    PID_ROLL_ANGLE.iDLimit = 0;

    PID_PITCH_ANGLE.kP = 0;
    PID_PITCH_ANGLE.kI = 0;
    PID_PITCH_ANGLE.kD = 0;
    PID_PITCH_ANGLE.iULimit = 0;
    PID_PITCH_ANGLE.iDLimit = 0;

    PID_YAW_ANGLE.kP = 0;
    PID_YAW_ANGLE.kI = 0;
    PID_YAW_ANGLE.kD = 0;
    PID_YAW_ANGLE.iULimit = 0;
    PID_YAW_ANGLE.iDLimit = 0;
}

void Controller_Moto(uint16_t baseSpeed,int16_t targetRoll,int16_t targetPitch,int16_t targetYaw, IMU_DATA_t *measure)
{

    if (measure->pitch > 45 || measure->pitch < -45 || measure->roll > 45 || measure->roll < -45)
    {
        PROTECT_LOCK = 1;
        printf("power lock\r\n");
    }

    if (PROTECT_LOCK >= 1){
        motoConfig.LU.speed = 0;
        motoConfig.LB.speed = 0;
        motoConfig.RU.speed = 0;
        motoConfig.RB.speed = 0;
        MOTO_SetSpeed(&motoConfig);
        return;
    }

    PID_Cal(&PID_ROLL_ANGLE,targetRoll,measure->roll,baseSpeed);
    PID_Cal(&PID_PITCH_ANGLE,targetPitch,measure->pitch,baseSpeed);
    PID_Cal(&PID_YAW_ANGLE,targetYaw,measure->yaw,baseSpeed);


    if(baseSpeed == 0)
    {
        motoConfig.LU.speed = 0;
        motoConfig.LB.speed = 0;
        motoConfig.RU.speed = 0;
        motoConfig.RB.speed = 0;
    } else
    {
//        printf("%d\r\n",(int16_t)PID_ROLL_ANGLE.output);
        motoConfig.LU.speed = (int16_t)baseSpeed + (int16_t)( + PID_ROLL_ANGLE.output + PID_PITCH_ANGLE.output - PID_YAW_ANGLE.output);
        motoConfig.LB.speed = (int16_t)baseSpeed + (int16_t)( + PID_ROLL_ANGLE.output - PID_PITCH_ANGLE.output + PID_YAW_ANGLE.output);
        motoConfig.RU.speed = (int16_t)baseSpeed + (int16_t)( - PID_ROLL_ANGLE.output + PID_PITCH_ANGLE.output + PID_YAW_ANGLE.output);
        motoConfig.RB.speed = (int16_t)baseSpeed + (int16_t)( - PID_ROLL_ANGLE.output - PID_PITCH_ANGLE.output - PID_YAW_ANGLE.output);
//        printf("%ld,%ld,%ld,%ld\r\n",motoConfig.LU.speed,motoConfig.LB.speed,motoConfig.RU.speed,motoConfig.RB.speed);

    }
    MOTO_SetSpeed(&motoConfig);
}