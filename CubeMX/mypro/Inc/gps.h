#ifndef __gps_H
#define __gps_H

#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "main.h"

#ifdef __cplusplus
extrn "C"{
#endif

#define false	0
#define true	1

/* 定义数组长度 */
#define GPS_Buffer_Length	210
#define UTCTime_Length		11
#define latitude_Length		11
#define N_S_Length			2
#define longitude_Length	12
#define E_W_Length			2
#define Ground_Speed_Length  5
#define Ground_Course_Length 5

typedef struct SaveData
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;
	char isParseData;
	char UTCTime[UTCTime_Length];
	char latitude[latitude_Length];
	char N_S[N_S_Length];
	char longitude[longitude_Length];
	char E_W[E_W_Length];
	char Ground_Speed[Ground_Speed_Length];
	char Ground_Course[Ground_Course_Length];
	char isUsefull;
}_SaveData;

extern _SaveData Save_Data;
extern uint8_t GPGSAFlag;
extern uint8_t GPGSABUffer[65];

extern uint8_t GPGSVFlag;
extern uint8_t GPGSVBuffer[210];

extern uint8_t PDOPString[4];
extern uint8_t SIViewString[2];

void GPS_dataPrase(void);
void clrStruct(void);

#ifdef __cplusplus
}
#endif
#endif
