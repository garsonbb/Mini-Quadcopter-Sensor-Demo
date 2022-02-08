/**
* @file         air530z.h
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#ifndef UAV_AIR530Z_H
#define UAV_AIR530Z_H

#include "stm32f4xx_hal.h"

#define AIR530Z_CFG_BAUDRATE_9600               "$PCAS01,1*1D\r\n"
#define AIR530Z_CFG_BAUDRATE_115200             "$PCAS01,5*19\r\n"
#define AIR530Z_CFG_UPDATERATE_1HZ              "$PCAS02,1000*2E\r\n"
#define AIR530Z_CFG_UPDATERATE_2HZ              "$PCAS02,500*1A\r\n"
#define AIR530Z_CFG_UPDATERATE_4HZ              "$PCAS02,250*18\r\n"
#define AIR530Z_CFG_UPDATERATE_5HZ              "$PCAS02,200*1D\r\n"
#define AIR530Z_CFG_UPDATERATE_10HZ             "$PCAS02,100*1E\r\n"
#define AIR530Z_CFG_NMEA_ONLY_GGA_RMC           "$PCAS03,1,0,0,0,1,0,0,0,0,0,,,0,0*02\r\n"
#define AIR530Z_CFG_SYSTEM_GPS_BDS_GLONASS      "$PCAS04,7*1E\r\n"
#define AIR530Z_CFG_SYSTEM_GPS                  "$PCAS04,1*18\r\n"
#define AIR530Z_CFG_SYSTEM_BDS                  "$PCAS04,2*1B\r\n"
#define AIR530Z_CFG_SYSTEM_GLONASS              "$PCAS04,4*1D\r\n"
#define AIR530Z_CFG_SYSTEM_GPS_BDS              "$PCAS04,3*1A\r\n"
#define AIR530Z_CFG_SYSTEM_GPS_GLONASS          "$PCAS04,5*1C\r\n"
#define AIR530Z_CFG_SYSTEM_BDS_GLONASS          "$PCAS04,6*1F\r\n"

#define AIR530Z_BUF_LIST_LENGTH                  2
#define AIR530Z_BUF_LIST_ITEM_LENGTH             80
#define AIR530Z_BUF_ALL_LENGTH                   180

typedef enum {
    AIR530Z_RESULT_OK = 0x01,
    AIR530Z_RESULT_FAIL,
    AIR530Z_RESULT_LOSTCONN,
    AIR530Z_RESULT_NOUPDATE
}AIR530Z_RESULT_t;

typedef struct {
    char GGA[5];
    float UTCtime;
    float lat;
    char uLat;
    float lon;
    char uLon;
    uint8_t FS;
    uint8_t numSv;
    float HDOP;
    float msl;
    char uMsl;
    float sep;
    float uSep;
    float diffAge;
    uint8_t diffSta;
}AIR530Z_GGA_t;

typedef struct {
    char RMC[5];
    float UTCtime;
    char status;
    float lat;
    char uLat;
    float lon;
    char uLon;
    float spd;
    float cog;
    uint32_t date;
    float mv;
    char mvE;
    char mode;
    char navStatus;
}AIR530Z_RMC_t;

typedef struct
{
    UART_HandleTypeDef *huart;
}AIR530Z_CONFIG_t;

AIR530Z_RESULT_t AIR530Z_Initialize(AIR530Z_CONFIG_t *air530ZConfig);
void AIR530Z_IRQ(AIR530Z_CONFIG_t *air530ZConfig);
void AIR530Z_Update(void);
AIR530Z_RESULT_t AIR530Z_GetLocation(AIR530Z_GGA_t *air530ZGga, AIR530Z_RMC_t *air530ZRmc);
AIR530Z_RESULT_t AIR530Z_Config(AIR530Z_CONFIG_t *air530ZConfig, char cfg[]);
#endif //UAV_AIR530Z_H
