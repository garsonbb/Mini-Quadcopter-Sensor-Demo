/**
* @file         IMU.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,11
* @version	    v1.0
*/
#ifndef UAV_IMU_H
#define UAV_IMU_H

typedef struct
{
    float roll;
    float pitch;
    float yaw;
}IMU_DATA_t;

//---------------------------------------------------------------------------------------------------
// Function declarations

void IMU_UPDATE(float b_gx, float b_gy, float b_gz, float b_ax, float b_ay, float b_az);

#endif //UAV_IMU_H
