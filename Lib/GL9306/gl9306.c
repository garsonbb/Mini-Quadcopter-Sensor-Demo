/**
* @file         gl9306.c
* @brief		This is a brief description.
* @details	    This is the detail description.
* @author		Garson
* @date		    2021 ,06
* @version	    v1.0
*/
#include "gl9306.h"
#include "retarget.h"

uint8_t GL9306_g_BUF[2][9] = {0};
uint8_t GL9306_g_BUF_UPADTED = 0;
uint8_t GL9306_g_POS = 0;

GL9306_RESULT_t GL9306_Initialize(GL9306_CONFIG_t *gl9306Config) {
    if (HAL_UARTEx_ReceiveToIdle_DMA(gl9306Config->huart,(uint8_t *)&GL9306_g_BUF, 18) == HAL_OK)
    { return GL9306_RESULT_OK; }
    return GL9306_RESULT_FAIL;
}

void GL9306_IRQ(GL9306_CONFIG_t *gl9306Config) {
//    printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x \r\n",GL9306_g_BUF[GL9306_g_POS][0],GL9306_g_BUF[GL9306_g_POS][1],GL9306_g_BUF[GL9306_g_POS][2],GL9306_g_BUF[GL9306_g_POS][3],GL9306_g_BUF[GL9306_g_POS][4],GL9306_g_BUF[GL9306_g_POS][5],GL9306_g_BUF[GL9306_g_POS][6],GL9306_g_BUF[GL9306_g_POS][7],GL9306_g_BUF[GL9306_g_POS][8]);
    GL9306_g_POS++;
    GL9306_g_POS = GL9306_g_POS & 1;
    GL9306_g_BUF_UPADTED = 1;
    HAL_UARTEx_ReceiveToIdle_DMA(gl9306Config->huart,(uint8_t *)&GL9306_g_BUF[GL9306_g_POS], 18);
}

GL9306_RESULT_t GL9306_Get(GL9306_DATA_t *gl9306Data) {
    uint8_t checkSum = 0;
    uint8_t pos = (GL9306_g_POS + 1) & 1;

    if (GL9306_g_BUF_UPADTED == 0) {
        return GL9306_RESULT_NOUPDATE;
    }

    for (int i = 2; i < 6; i++) {
        checkSum += GL9306_g_BUF[pos][i];
    }
    if (checkSum != GL9306_g_BUF[pos][6])
    {
        return GL9306_RESULT_FAIL;
    }

//    printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x \r\n",GL9306_g_BUF[pos][0],GL9306_g_BUF[pos][1],GL9306_g_BUF[pos][2],GL9306_g_BUF[pos][3],GL9306_g_BUF[pos][4],GL9306_g_BUF[pos][5],GL9306_g_BUF[pos][6],GL9306_g_BUF[pos][7],GL9306_g_BUF[pos][8]);

    gl9306Data->DeltaX = (int16_t) ((GL9306_g_BUF[pos][3] << 8) | GL9306_g_BUF[pos][2]);
    gl9306Data->DeltaY = (int16_t) ((GL9306_g_BUF[pos][5] << 8) | GL9306_g_BUF[pos][4]);
    GL9306_g_BUF_UPADTED = 0;

    return GL9306_RESULT_OK;
}
