#include "ubloxgps.h"
#include "usart.h"


uint8_t revDataBuffer[10];

char     rxdatabufer;
uint16_t point1 = 0;

_SaveData Save_Data;

//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while(0 == (USART1->SR & 0X40));
	USART1->DR = (uint8_t) ch;
	return ch;
}

#if EN_USART1_RX
char USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0;

void GPS_Init(void)
{
	MX_USART1_UART_Init();
	CLR_Buf();
	HAL_UART_Receive_IT(&huart1, revDataBuffer, 1);
}

uint8_t Hand(char *a)	//串口命令识别函数
{
	if(strstr(USART_RX_BUF,a) != NULL)
		return 1;
	else
		return 0;
}

void CLR_Buf(void)
{
	memset(USART_RX_BUF, 0, USART_REC_LEN);
	point1 = 0;
}

void GPS_errorLog(int num)
{
	while(1)
	{
		printf("ERROR%d\r\n",num);
	}
}

void GPS_parseGpsBuffer(void)
{
	char *subString;
	char *subStringNext;
	char i = 0;
	 if(Save_Data.isGetData)
	 {
		 Save_Data.isGetData = false;
		 printf("*********************\r\n");
		 //printf(Save_Data.GPS_Buffer);
		 
		 for(i = 0; i <= 6; i++)
		 {
			if(0 == i)
			{
				if(NULL == (subString = strstr(Save_Data.GPS_Buffer,",")))
				{
					GPS_errorLog(1);//解析错误
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
					GPS_errorLog(2);//解析错误
				}
			}
			
		 }
	 }
}


void GPS_printGpsBuffer(void)
{
	if(Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		/* printf() */
		
		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			/* printf GSP information */
			
		}
		else
		{
			//printf("GPS DATA is not usefull! \r\n");
		}
	}
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
}

void GPS_Callback(void)
{
    uint32_t timeout = 0;
	while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY)
	{
	 timeout++;
     if(timeout>HAL_MAX_DELAY) break;		
	}
		
//	while(0 == (USART1->SR & 0X40));
	USART1->DR = revDataBuffer[0];	
	
	while(HAL_UART_Receive_IT(&huart1, revDataBuffer, 1) != HAL_OK)
	{
	 timeout++;
	 if(timeout>HAL_MAX_DELAY) break;	
	}	
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
	
	if(USART1 == huart->Instance)
	{
		while(0 == (USART1->SR & 0X40));
		USART1->DR = revDataBuffer[0];
		if('$' == revDataBuffer[0])
		{
			point1 = 0;
		}
		
		USART_RX_BUF[point1++] = revDataBuffer[0];
		/* $GPRMC */
		if('$' == USART_RX_BUF[0] && 'M' == USART_RX_BUF[4] && 'C' == USART_RX_BUF[5])
		{
			if('\n' == revDataBuffer[0])
			{
				memset(Save_Data.GPS_Buffer,0,GPS_Buffer_Length);
				memcpy(Save_Data.GPS_Buffer,USART_RX_BUF,point1);
				Save_Data.isGetData = true;
				point1 = 0;
				memset(USART_RX_BUF, 0 , USART_REC_LEN);
			}
		}
		
		/* $GPGSA */
		if('$' == USART_RX_BUF[0] && 'S' == USART_RX_BUF[4] && 'A' == USART_RX_BUF[5])
		{
			if('\n' == revDataBuffer[0])
			{
				
			}
		}
		
		if(point1 >= USART_REC_LEN)
		{
			point1 = USART_REC_LEN;
		}
		
		while(HAL_OK != HAL_UART_Receive_IT(&huart1, revDataBuffer, 1));
	}
}

#endif
