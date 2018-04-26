#include "gps.h"
#include "usart.h"
#include "string.h"
#include "gpio.h"
#include "stdlib.h"
#include "stdio.h"

extern float gSpeed;
extern double gDistance;
extern uint8_t gInitFlag;
extern uint8_t gNGPSflag;
extern uint8_t UTC[30];
extern uint8_t LatLongInfo[30];
extern uint8_t PositingInfo[30];
extern uint8_t GroundSpeedInfo[30];
extern uint8_t GroundCourseinfo[30];

_SaveData Save_Data;

uint8_t GPSDataFlag;

uint8_t GPGSAFlag;
uint8_t GPGSABUffer[65];

uint8_t GPGSVFlag;
uint8_t GPGSVBuffer[210];

uint8_t PDOPString[4];
uint8_t SIViewString[2];

static void GPS_PraseGPGSA(void)
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if(0 == GPGSAFlag)
	{
		return;
	}
	GPGSAFlag = 0;
	subString = (char*)GPGSABUffer;
	while(NULL != (subStringNext = strstr(subString,",")))
	{
		i++;
		if(16 == i)
		{
			memcpy(PDOPString,subString,subStringNext-subString);
			break;
		}
		subString = subStringNext;
		subString++;
	}
	//
}

static void GPS_PraseGPGSV(void)
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if(0 == GPGSVFlag)
	{
		return;
	}
	GPGSVFlag = 0;
	subString = (char*)GPGSVBuffer;
	while(NULL != (subStringNext = strstr(subString,",")))
	{
		i++;
		if(4 == i)
		{
			memcpy(SIViewString,subString,subStringNext-subString);
			break;
		}
		subString = subStringNext;	
	}	
	//
	
}

void GPS_dataPrase()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	uint8_t cmd[2];
	cmd[0] = 0x52;
	
	if(0 == gRevFlag)
	{
		return;
	}
	
	/* $GPGSV */
	if(1 == gRevFlag)
	{
		memset(GPGSVBuffer,0,210);
		memcpy(GPGSVBuffer,RevRxDataBuffer,gCount);
		gRevFlag = 0;
		gCount = 0;
		GPGSVFlag = 1;
		memset(RevRxDataBuffer,0,USART_REC_LEN);
	}
	/* $GPRMC */
	if(2 == gRevFlag)
	{
		memset(Save_Data.GPS_Buffer,0,GPS_Buffer_Length);
		memcpy(Save_Data.GPS_Buffer,RevRxDataBuffer,gCount);
		gRevFlag = 0;
		gCount = 0;
		Save_Data.isGetData = true;
		memset(RevRxDataBuffer,0,USART_REC_LEN);
		
	}
	/* $GPGSA */
	if(3 == gRevFlag)
	{
		memset(GPGSABUffer,0,65);
		memcpy(GPGSABUffer,RevRxDataBuffer,gCount);
		gRevFlag = 0;
		gCount = 0;
		GPGSAFlag = 1;
		memset(RevRxDataBuffer,0,USART_REC_LEN);
	}
	
	GPS_PraseGPGSA();
	GPS_PraseGPGSV();
	
	 if(Save_Data.isGetData)
	 {
		 Save_Data.isGetData = false;
		 
		 for(i = 0; i <= 8; i++)
		 {
			if(0 == i)
			{
				if(NULL == (subString = strstr(Save_Data.GPS_Buffer,",")))
				{
					//GPS_errorLog(1);//解析错误
				}
			}
			else
			{
				subString++;
				if(NULL != (subStringNext = strstr(subString,",")))
				{
					char usefullBuffer[2];
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//get UTC Time
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//get UTC time
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取维度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//get N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//Get E/W
						case 7:memcpy(Save_Data.Ground_Speed,subString,subStringNext-subString);break;	//获取地面速率
						case 8:memcpy(Save_Data.Ground_Course,subString,subStringNext-subString);break;	//获取地面航向，一真北为参考基准
							
						default:break;
					}
					
					subString = subStringNext;
					Save_Data.isParseData = true;
					if('A' == usefullBuffer[0])
						Save_Data.isUsefull = true;
					else if('V' == usefullBuffer[0])
						Save_Data.isUsefull = false;
				}
				else
				{
					//GPS_errorLog(2);//解析错误
				}
			}
			
		 }
	 }
	 
	if(Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		GPSDataFlag = 1;
		/* printf() */
//		printf("UTC:%s",Save_Data.UTCTime);
		sprintf((char*)UTC,"UTC:%s",Save_Data.UTCTime);
		
		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			/* printf GSP information */
			sprintf((char *)PositingInfo,"PDOP:%s Satellites:%s MODE:%c",PDOPString,SIViewString,'G');
			sprintf((char *)LatLongInfo,"%s:%s %s:%s",Save_Data.latitude,Save_Data.E_W,Save_Data.longitude,Save_Data.N_S);
			sprintf((char *)GroundSpeedInfo,"%s knot",Save_Data.Ground_Speed);
			gSpeed = atof(Save_Data.Ground_Speed);//速度
			
			gInitFlag = 1;
			gNGPSflag = 0;
			gDistance = 0.0;
		}
		else
		{
			//printf("GPS DATA is not usefull! \r\n");
			sprintf((char *)PositingInfo,"PDOP:%s Satellites:%s MODE:%c",PDOPString,SIViewString,'N');
			sprintf((char *)LatLongInfo,"%s:%s %s:%s",Save_Data.latitude,Save_Data.E_W,Save_Data.longitude,Save_Data.N_S);
			sprintf((char *)GroundSpeedInfo,"%s knot",Save_Data.Ground_Speed);
			
			gSpeed = atof(Save_Data.Ground_Speed);//速度
			
			/* 角度初始化 */
			if(0 == gNGPSflag)
			{
				HAL_UART_Transmit_IT(&huart2,cmd,1);
				gNGPSflag = 1;
			}
		}	
	}
	
	clrStruct();
}

void clrStruct()
{
	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
	memset(Save_Data.GPS_Buffer,0,GPS_Buffer_Length);
	memset(Save_Data.UTCTime, 0, UTCTime_Length);
	memset(Save_Data.latitude,0,latitude_Length);
	memset(Save_Data.N_S, 0, N_S_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
	memset(Save_Data.E_W, 0, E_W_Length);
	memset(Save_Data.Ground_Speed,0,Ground_Speed_Length);
	memset(Save_Data.Ground_Course,0,Ground_Course_Length);
}

