/**
* @file         filter.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,02
* @version	    v1.0
*/
#include "filter.h"

float LowPassFilter(FILTER_DATA_t *filterData ,float x)
{
    float y=0;
    float coex = 0.2066f;
    float coex1 = 0.4131f;
    float coex2 = 0.2066f;
    float coey1 = 0.3695f;
    float coey2 = -0.1958f;
    y = coex * x + coex1 * filterData->x1 + coex2 * filterData->x2 + coey1 * filterData->y1 + coey2 * filterData->y2;
    filterData->x2 = filterData->x1;
    filterData->x1 = x;
    filterData->y2 = filterData->y1;
    filterData->y1 = y;
    return y;
}

float HighPassFilter(FILTER_DATA_t *filterData ,float x)
{
    float y=0;
    float coex = 0.3913f;
    float coex1 = -0.7827f;
    float coex2 = 0.3913f;
    float coey1 = 0.3695f;
    float coey2 = -0.1958f;
    y = coex * x + coex1 * filterData->x1 + coex2 * filterData->x2 + coey1 * filterData->y1 + coey2 * filterData->y2;
    filterData->x2 = filterData->x1;
    filterData->x1 = x;
    filterData->y2 = filterData->y1;
    filterData->y1 = y;
    return y;
}
