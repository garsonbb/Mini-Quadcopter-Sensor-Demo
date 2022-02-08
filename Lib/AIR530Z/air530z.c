/**
* @file         air530z.c
* @brief		This is a brief description.
* @details	    NMEA0183 Protocol.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#include "air530z.h"
#include "usart.h"
#include "retarget.h"

#include <stdlib.h>
#include "string.h"

#define _REPLACE0(c) ((c) > ('0') ? (c):('x'))

int mstrcmp(const char *s1, const char *s2);
uint8_t mstrlen(const char *s);
uint16_t mstrenter(const char *s ,uint16_t start);

unsigned char AIR530Z_g_BUF_LIST[AIR530Z_BUF_LIST_LENGTH][AIR530Z_BUF_LIST_ITEM_LENGTH]={0};
unsigned char AIR530Z_g_BUF_ALL[AIR530Z_BUF_ALL_LENGTH]={0};
uint8_t AIR530Z_g_BUF_UPADTED=0;

AIR530Z_RESULT_t AIR530Z_Initialize(AIR530Z_CONFIG_t *air530ZConfig)
{
    if (HAL_UARTEx_ReceiveToIdle_DMA(air530ZConfig->huart,(uint8_t *)&AIR530Z_g_BUF_ALL, 360) == HAL_OK) //start to receiving
    {
        AIR530Z_Config(air530ZConfig,AIR530Z_CFG_BAUDRATE_115200);
        return AIR530Z_RESULT_OK;
    }
    return AIR530Z_RESULT_FAIL;
}

void AIR530Z_IRQ(AIR530Z_CONFIG_t *air530ZConfig)
{
    AIR530Z_g_BUF_UPADTED = 1;
    HAL_UARTEx_ReceiveToIdle_DMA(air530ZConfig->huart,(uint8_t *)&AIR530Z_g_BUF_ALL, 360);
}

AIR530Z_RESULT_t AIR530Z_GetLocation(AIR530Z_GGA_t *air530ZGga, AIR530Z_RMC_t *air530ZRmc)
{
    unsigned char buf_comma_split[15][15]={0};
    uint8_t buf_comma_split_row=0;
    uint8_t buf_comma_split_pos=0;
    uint8_t length=0;
    uint8_t sumCheckStr[2] = {0};
    uint8_t sumCheckInt = 0;
    uint8_t flagSuming = 0;
    uint16_t enterPos1=0;
    uint16_t enterPos2=0;

    if (AIR530Z_g_BUF_UPADTED == 0)
    {
        return AIR530Z_RESULT_NOUPDATE;
    }

    if(AIR530Z_g_BUF_ALL[0]!='$')
    {
        return AIR530Z_RESULT_FAIL;
    }

    enterPos1 = mstrenter((char *) AIR530Z_g_BUF_ALL , 0) + 1;// +1复制字节个数,从下标到个数.
//    printf("pos1: %d,%d\r\n",enterPos1,AIR530Z_BUF_LIST_ITEM_LENGTH-enterPos1);//del
    memcpy(AIR530Z_g_BUF_LIST[0], AIR530Z_g_BUF_ALL, enterPos1);
    memset(&AIR530Z_g_BUF_LIST[0][enterPos1],0,AIR530Z_BUF_LIST_ITEM_LENGTH-enterPos1);// reset 0,恰好是从\r\n后清0.

    enterPos2 = mstrenter((char *) AIR530Z_g_BUF_ALL , enterPos1) + 1;
//    printf("pos2: %d,%d,%d\r\n",enterPos2,enterPos2 - enterPos1,AIR530Z_BUF_LIST_ITEM_LENGTH-(enterPos2 - enterPos1));//del
    memcpy(AIR530Z_g_BUF_LIST[1], &AIR530Z_g_BUF_ALL[enterPos1], enterPos2 - enterPos1);
    memset(&AIR530Z_g_BUF_LIST[1][enterPos2 - enterPos1],0,AIR530Z_BUF_LIST_ITEM_LENGTH-(enterPos2 - enterPos1));// reset 0

//    printf("Data1: %s\r\n",AIR530Z_g_BUF_LIST[0]);
//    printf("Data2: %s\r\n",AIR530Z_g_BUF_LIST[1]);

    for (int listRow = 0; listRow < AIR530Z_BUF_LIST_LENGTH; ++listRow) {

        buf_comma_split_row=0;
        buf_comma_split_pos=0;
        length=0;
        sumCheckInt=0;
        flagSuming = 0;

        if (AIR530Z_g_BUF_LIST[listRow][3] == 'G')//GGA
        {
            length = mstrlen((char *) AIR530Z_g_BUF_LIST[listRow]);
            for (int pos = 0; pos < length; ++pos) //split
            {
//                printf("%c",AIR530Z_g_BUF_LIST[listRow][pos]);
                if(AIR530Z_g_BUF_LIST[listRow][pos] == ',')
                {
                    buf_comma_split_row++;
                    buf_comma_split_pos=0;
                }
                else
                {
                    buf_comma_split[buf_comma_split_row][buf_comma_split_pos] = AIR530Z_g_BUF_LIST[listRow][pos];
                    buf_comma_split_pos++;
//                    printf("%c ",AIR530Z_g_BUF_LIST[listRow][pos]);
                }

                if(AIR530Z_g_BUF_LIST[listRow][pos] == '*')
                {
                    flagSuming = 1;
                }
                if(flagSuming == 0)
                {
                    sumCheckInt ^= AIR530Z_g_BUF_LIST[listRow][pos];
                }
            }
//            printf("\r\n");
            sumCheckStr[0] = AIR530Z_g_BUF_LIST[listRow][length - 2];
            sumCheckStr[1] = AIR530Z_g_BUF_LIST[listRow][length - 1];
            if(sumCheckInt == strtol((char *)sumCheckStr, NULL, 16))
            {
//                printf("check ok\r\n");
            }

            //datatype transform
            //    char GGA[5];
            length = mstrlen((char *)buf_comma_split[0]);
            for (int t = 0; t < length; ++t) {
                air530ZGga->GGA[t]=buf_comma_split[0][t];
            }
//            printf("%s\r\n",air530ZGga->GGA);
            //    float UTCtime;hhmmss.sss
            air530ZGga->UTCtime=strtof((char *)buf_comma_split[1], NULL);
//            printf("%f\r\n",air530ZGga->UTCtime);
            //    float lat;ddmm.mmmmm
            air530ZGga->lat= strtof((char *)buf_comma_split[2], NULL) / 100.f;
//            printf("%f\r\n",air530ZGga->lat);
            //    char uLat;
            air530ZGga->uLat = buf_comma_split[3][0];
//            printf("%c\r\n",air530ZGga->uLat);
            //    float lon;dddmm.mmmmm
            air530ZGga->lon= strtof((char *)buf_comma_split[4], NULL) / 100.f;
//            printf("%f\r\n",air530ZGga->lon);
            //    char uLon;
            air530ZGga->uLon = buf_comma_split[5][0];
//            printf("%c\r\n",air530ZGga->uLon);
            //    uint8_t FS;d
            air530ZGga->FS = strtol((char *)buf_comma_split[6], NULL, 10);
//            printf("%d\r\n",air530ZGga->FS);
            //    uint8_t numSv;dd
            air530ZGga->numSv=strtol((char *)buf_comma_split[7], NULL, 10);
//            printf("%d\r\n",air530ZGga->numSv);
            //    float HDOP;
            air530ZGga->HDOP=strtof((char *)buf_comma_split[8], NULL);
//            printf("%f\r\n",air530ZGga->HDOP);
            //    float msl;
            air530ZGga->msl=strtof((char *)buf_comma_split[9], NULL);
//            printf("%f\r\n",air530ZGga->msl);
            //    char uMsl;
            air530ZGga->uMsl = buf_comma_split[10][0];
//            printf("%c\r\n",air530ZGga->uMsl);
            //    float sep;
            air530ZGga->sep=strtof((char *)buf_comma_split[11], NULL);
//            printf("%f\r\n",air530ZGga->sep);
            //    float uSep;
            air530ZGga->uSep=strtof((char *)buf_comma_split[12], NULL);
//            printf("%f\r\n",air530ZGga->uSep);
            //    float diffAge;
            air530ZGga->diffAge=strtof((char *)buf_comma_split[13], NULL);
//            printf("%f\r\n",air530ZGga->diffAge);
            //    uint8_t diffSta;
            air530ZGga->diffSta=strtol((char *)buf_comma_split[14], NULL, 10);
//            printf("%d\r\n",air530ZGga->diffSta);
        }


        if (AIR530Z_g_BUF_LIST[listRow][3] == 'R')//RMC
        {
            length = mstrlen((char *) AIR530Z_g_BUF_LIST[listRow]);
            for (int pos = 0; pos < length; ++pos) //split
            {
//                printf("%c",AIR530Z_g_BUF_LIST[listRow][pos]);
                if(AIR530Z_g_BUF_LIST[listRow][pos] == ',')
                {
                    buf_comma_split_row++;
                    buf_comma_split_pos=0;
                }
                else
                {
                    buf_comma_split[buf_comma_split_row][buf_comma_split_pos] = AIR530Z_g_BUF_LIST[listRow][pos];
                    buf_comma_split_pos++;
//                    printf("%c",AIR530Z_g_BUF_LIST[listRow][pos]);
                }

                if(AIR530Z_g_BUF_LIST[listRow][pos] == '*')
                {
                    flagSuming = 1;
                }
                if(flagSuming == 0)
                {
                    sumCheckInt ^= AIR530Z_g_BUF_LIST[listRow][pos];
                }
            }
//            printf("\r\n");
            sumCheckStr[0] = AIR530Z_g_BUF_LIST[listRow][length - 2];
            sumCheckStr[1] = AIR530Z_g_BUF_LIST[listRow][length - 1];
            if(sumCheckInt == strtol((char *)sumCheckStr, NULL, 16))
            {
//                printf("check ok\r\n");
            }

            //datatype transform
            //    char GGA[5];
            length = mstrlen((char *)buf_comma_split[0]);
            for (int t = 0; t < length; ++t) {
                air530ZRmc->RMC[t]=buf_comma_split[0][t];
            }
//            printf("%s\r\n",air530ZRmc->RMC);
            //    float UTCtime;hhmmss.sss
            air530ZRmc->UTCtime=strtof((char *)buf_comma_split[1], NULL);
//            printf("%f\r\n",air530ZRmc->UTCtime);
            //    char status;
            air530ZRmc->status=buf_comma_split[2][0];
//            printf("%c\r\n",air530ZRmc->status);
            //    float lat;ddmm.mmmmm
            air530ZRmc->lat= strtof((char *)buf_comma_split[3], NULL) / 100.f;
//            printf("%f\r\n",air530ZRmc->lat);
            //    char uLat;
            air530ZRmc->uLat = _REPLACE0(buf_comma_split[4][0]);
//            printf("%c\r\n",air530ZRmc->uLat);
            //    float lon;dddmm.mmmmm
            air530ZRmc->lon= strtof((char *)buf_comma_split[5], NULL) / 100.f;
//            printf("%f\r\n",air530ZRmc->lon);
            //    char uLon;
            air530ZRmc->uLon = _REPLACE0(buf_comma_split[6][0]);
//            printf("%c\r\n",air530ZRmc->uLon);
            //    float spd;
            air530ZRmc->spd = strtof((char *)buf_comma_split[7], NULL);
//            printf("%f\r\n",air530ZRmc->spd);
            //    float cog;
            air530ZRmc->cog = strtof((char *)buf_comma_split[8], NULL);
//            printf("%f\r\n",air530ZRmc->cog);
            //    uint32_t date;
            air530ZRmc->date = strtol((char *)buf_comma_split[9], NULL, 10);
//            printf("%lu\r\n",air530ZRmc->date);
            //    float mv;
            air530ZRmc->mv = strtof((char *)buf_comma_split[10], NULL);
//            printf("%f\r\n",air530ZRmc->mv);
            //    char mvE;
            air530ZRmc->mvE=buf_comma_split[11][0];
//            printf("%c\r\n",air530ZRmc->mvE);
            //    char mode;
            air530ZRmc->mode=buf_comma_split[12][0];
//            printf("%c\r\n",air530ZRmc->mode);
            //    char navStatus;
            air530ZRmc->navStatus=buf_comma_split[13][0];
//            printf("%c\r\n",air530ZRmc->navStatus);
        }
    }

    AIR530Z_g_BUF_UPADTED = 0;

    if(air530ZRmc->status == 'V' || air530ZGga->FS == 0)
    {
        return AIR530Z_RESULT_LOSTCONN;
    }

    return AIR530Z_RESULT_OK;
}

AIR530Z_RESULT_t AIR530Z_Config(AIR530Z_CONFIG_t *air530ZConfig, char cfg[])
{
    if(mstrcmp(cfg,AIR530Z_CFG_BAUDRATE_115200) == 0)
    {
        HAL_UART_Transmit(air530ZConfig->huart,(uint8_t *)cfg,mstrlen(cfg),0xffff);
        HAL_UART_DeInit(air530ZConfig->huart);
        air530ZConfig->huart->Init.BaudRate=115200;
        HAL_UART_Init(air530ZConfig->huart);
        if (HAL_UARTEx_ReceiveToIdle_DMA(air530ZConfig->huart,(uint8_t *)&AIR530Z_g_BUF_ALL, 360) == HAL_OK)
        {return AIR530Z_RESULT_OK;}
    }

    if(mstrcmp(cfg,AIR530Z_CFG_BAUDRATE_9600) == 0)
    {
        HAL_UART_Transmit(air530ZConfig->huart,(uint8_t *)cfg,mstrlen(cfg),0xffff);
        HAL_UART_DeInit(air530ZConfig->huart);
        air530ZConfig->huart->Init.BaudRate=9600;
        HAL_UART_Init(air530ZConfig->huart);
        if (HAL_UARTEx_ReceiveToIdle_DMA(air530ZConfig->huart,(uint8_t *)&AIR530Z_g_BUF_ALL, 360) == HAL_OK)
        {return AIR530Z_RESULT_OK;}
    }

    if(HAL_UART_Transmit(air530ZConfig->huart,(uint8_t *)cfg,mstrlen(cfg),0xffff) == HAL_OK)
    {
        return AIR530Z_RESULT_OK;
    }
    return AIR530Z_RESULT_FAIL;
}

int mstrcmp(const char *s1, const char *s2)
{
    int i =0;
    while (*s1 && *s2 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *s1 -*s2;
}

uint8_t mstrlen(const char *s)
{
    int i =0;
    while ((*s++) != '\0')
    {
        i++;
    }
    return i;
}

uint16_t mstrenter(const char *s ,uint16_t start)
{
    int i =start;
    s=s+start;
    while ((*s) != '\r' || (*(s+1) != '\n'))
    {
        s++;
        i++;
    }
    return i+1;//返回\r\n中的\n的下标
}


