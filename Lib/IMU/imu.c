/**
* @file         IMU.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2020 ,11
* @version	    v1.0
*/
#include "imu.h"
#include "arm_math.h"
#include "math.h"
#include "retarget.h"

//----------------------------------------------------------------------------------------------------
// Variable declaration 1

extern volatile float q0, q1, q2, q3;    // quaternion of sensor frame relative to auxiliary frame
//extern volatile float roll, pitch;
extern volatile float b_x,b_y,b_z;
extern volatile float b_h,v0;

//---------------------------------------------------------------------------------------------------
// Variable definitions 2
volatile float Ki = 0.0f;
volatile float Kp = 1.f;
volatile float sampleTime = 1/250.f;//1/xHz
volatile float rad2angle = 57.3f;

volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;                    // quaternion of sensor frame relative to auxiliary frame
volatile float integralFBx = 0.000f, integralFBy = 0.000f, integralFBz = 0.000f;    // integral error terms scaled by Ki
IMU_DATA_t imuData;//全局变量调用
volatile float b_x, b_y, b_z = 0.f;



//b_,b系,body系,载体坐标系
//n_,n系,navigation系,东北天坐标系
void IMU_UPDATE(float b_gx, float b_gy, float b_gz, float b_ax, float b_ay, float b_az) {
    float normalize = 0.f;
    float n2b_ax, n2b_ay, n2b_az;
    float e_ax, e_ay, e_az;//e_,误差,n系重力加速度转换到b系下的叉积作为误差
    float qa, qb, qc;

    if (!((b_ax == 0.0f) && (b_ay == 0.0f) && (b_az == 0.0f))) {
        //归一化输入的b系加速度
        arm_sqrt_f32(b_ax * b_ax + b_ay * b_ay + b_az * b_az, &normalize);
        normalize = 1 / normalize;
        b_ax *= normalize;
        b_ay *= normalize;
        b_az *= normalize;
        //把n系重力加速度从转换到b系
        n2b_ax = 2 * (q1 * q3 - q0 * q2);
        n2b_ay = 2 * (q0 * q1 + q2 * q3);
        n2b_az = 2 * (q0 * q0 + q3 * q3) - 1.f;
        //求误差
        e_ax = (b_ay * n2b_az - b_az * n2b_ay);
        e_ay = (b_az * n2b_ax - b_ax * n2b_az);
        e_az = (b_ax * n2b_ay - b_ay * n2b_ax);
        //PI
        if (Ki > 0.0f) {
            integralFBx += Ki * e_ax * sampleTime;
            integralFBy += Ki * e_ay * sampleTime;
            integralFBz += Ki * e_az * sampleTime;
            b_gx += integralFBx;
            b_gy += integralFBy;
            b_gz += integralFBz;
        } else {
            integralFBx = 0.0f;
            integralFBy = 0.0f;
            integralFBz = 0.0f;
        }
        b_gx += Kp * e_ax;
        b_gy += Kp * e_ay;
        b_gz += Kp * e_az;
    }
    //一阶龙塔
    b_gx *= 0.5f * sampleTime;
    b_gy *= 0.5f * sampleTime;
    b_gz *= 0.5f * sampleTime;
    qa = q0;
    qb = q1;
    qc = q2;
    q0 += (-qb * b_gx - qc * b_gy - q3 * b_gz);
    q1 += (qa * b_gx + qc * b_gz - q3 * b_gy);
    q2 += (qa * b_gy - qb * b_gz + q3 * b_gx);
    q3 += (qa * b_gz + qb * b_gy - qc * b_gx);
    //归一化四元数
    arm_sqrt_f32(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3, &normalize);
    normalize = 1 / normalize;
    q0 *= normalize;
    q1 *= normalize;
    q2 *= normalize;
    q3 *= normalize;
    //四元数转换为姿态矩阵
//    imuData.roll = asinf(-2.f * (q1 * q3 - q0 * q2)) * rad2angle;
//    imuData.pitch = atan2f(2.f * (q2 * q3 + q0 * q1), (1.f - 2.f * (q1 * q1 + q2 * q2))) * rad2angle;
    //yaw1 = atan2f(2.f * (q1 * q2 - q0 * q3), 1.f - 2.f * (q2 * q2 + q3 * q3))* 57.3f;
//    imuData.yaw = atan2f(2.f * (q1 * q2 + q0 * q3), (q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3)) * rad2angle;

//    imuData.yaw = atan2f(2.f * (q1 * q2 - q0 * q3), (2.f * (q0 * q0 + q1 * q1)) - 1) * rad2angle;
//    imuData.roll = -asinf(2.f * (q1 * q3 + q0 * q2)) * rad2angle;
//    imuData.pitch = atan2f(2.f * (q2 * q3 - q0 * q1), (2.f * (q0 * q0 + q3 * q3)) - 1) * rad2angle;

    imuData.pitch = atan2f(2.f * (q2 * q3 + q0 * q1), (2.f * (q0 * q0 + q3 * q3)) - 1) * rad2angle;
    imuData.roll = -asinf(2.f * (q1 * q3 - q0 * q2)) * rad2angle;
    imuData.yaw = atan2f(2.f * (q1 * q2 + q0 * q3), (2.f * (q0 * q0 + q1 * q1)) - 1) * rad2angle;
}


