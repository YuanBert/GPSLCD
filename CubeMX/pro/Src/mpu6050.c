#include "mpu6050.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"

MPU6050_Info_t gMPU6050_Info;

float a[3];
float w[3];
float angle[3];
float T;
uint8_t Re_buf[11];
uint8_t temp_buf[11];
uint8_t counter;
uint8_t sign;

uint8_t RevBuf[2];
static uint8_t temp[11];

void HAL_MPU6050_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
	
  if(huart2.Instance == huart->Instance)
  {
	temp[counter] = RevBuf[0];
	if(0 == counter && 0x55 != temp[0])
	{
		HAL_UART_Receive_IT(&huart2,RevBuf,1);
		return;
	}
	counter++;
	if(11 == counter)
	{
		memcpy(Re_buf,temp,11);
		counter = 0;
		sign = 1;
	}
	HAL_UART_Receive_IT(&huart2,RevBuf,1);
  }
}

void MPU6050_Init(void)
{
	MX_USART2_UART_Init();
	HAL_UART_Receive_IT(&huart2,RevBuf,1);
	counter = 0;
	memset(Re_buf,0,11);
	memset(temp,0,11);
	return ;
}

void MPU6050_PraseData(void)
{
	uint8_t mTempBuf[11];
	if(1 == sign)
	{
		memcpy(mTempBuf,Re_buf,11);
		sign = 0;
		if(0x55 == mTempBuf[0])
		{
			switch(mTempBuf[1])
			{
				case 0x51:
					a[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*16;	//X����ٶ�
					a[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*16;	//Y����ٶ�
					a[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*16;	//Z����ٶ�
					T = ((short)(mTempBuf[9] << 8 | mTempBuf[8])) / 340.0+ 36.25;
					gMPU6050_Info.ax = a[0];
					gMPU6050_Info.ay = a[1];
					gMPU6050_Info.az = a[2];
					gMPU6050_Info.T = T;
					gMPU6050_Info.flag = 1;
					break;
				case 0x52:
					w[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*2000;	//X����ٶ�
					w[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*2000;	//Y����ٶ�
					w[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*2000;	//Z����ٶ�
					T = ((short)(mTempBuf[9] << 8 | mTempBuf[8])) / 340.0+ 36.25;
					gMPU6050_Info.wx = w[0];
					gMPU6050_Info.wy = w[1];
					gMPU6050_Info.wz = w[2];
					gMPU6050_Info.T = T;
					gMPU6050_Info.flag = 1;
					break;
				case 0x53:
					angle[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*180;	//X���ת��
					angle[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*180;	//Y�ḩ����
					angle[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*180;	//Z�ẽƫ��
					T = ((short)(mTempBuf[9] << 8 | mTempBuf[8])) / 340.0+ 36.25;
					gMPU6050_Info.anglex = angle[0];
					gMPU6050_Info.angley = angle[1];
					gMPU6050_Info.anglez = angle[2];
					gMPU6050_Info.T = T;
					gMPU6050_Info.flag = 1;
					break;
				default:break;
			}
		}
	}
}

/*************** END OF FILE *************/
