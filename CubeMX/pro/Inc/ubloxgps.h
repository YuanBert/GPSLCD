#ifndef __ubloxgps_H
#define __ubloxgps_H

#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "main.h"

#ifdef __cplusplus
extrn "C"{
#endif

#define USART_REC_LEN			210
#define EN_USART1_RX			1

extern char USART_RX_BUF[USART_REC_LEN];
extern uint16_t	USART_RX_STA;

#define false	0
#define true	1

/* 定义数组长度 */
#define GPS_Buffer_Length	80
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

extern char     rxdatabufer;
extern uint16_t point1;
extern _SaveData Save_Data;
//

void CLR_Buf(void);
uint8_t Hand(char *a);
void clrStruct(void);

void GPS_errorLog(int num);
void GPS_parseGpsBuffer(void);
void GPS_printGpsBuffer(void);


void GPS_Init(void);
#ifdef __cplusplus
}
#endif

#endif
