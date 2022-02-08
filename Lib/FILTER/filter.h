/**
* @file         filter.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#ifndef UAV_FILTER_H
#define UAV_FILTER_H

typedef struct {
    float x1;
    float x2;
    float y1;
    float y2;
}FILTER_DATA_t;

float LowPassFilter(FILTER_DATA_t *filterData ,float x);//return y;
float HighPassFilter(FILTER_DATA_t *filterData ,float x);//return y;

#endif //UAV_FILTER_H
