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
  uint8_t rev;
	
  if(huart2.Instance == huart->Instance)
  {
	if(RESET != __HAL_UART_GET_IT_SOURCE(&huart2,UART_IT_RXNE))
	{
		rev = USART2->DR;
	}
	
	temp[counter] = rev;
	if(0 == counter && 0x55 != temp[0])
	{
		return;
	}
	counter++;
	if(11 == counter)
	{
		memcpy(Re_buf,temp,11);
		counter = 0;
		sign = 1;
	}
  }
}

void MPU6050_Init(void)
{
	MX_USART2_UART_Init();
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
					a[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*16;	//X轴加速度
					a[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*16;	//Y轴加速度
					a[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*16;	//Z轴加速度
					T = ((short)(mTempBuf[9] << 8 | mTempBuf[8])) / 340.0+ 36.25;
					gMPU6050_Info.ax = a[0];
					gMPU6050_Info.ay = a[1];
					gMPU6050_Info.az = a[2];
					gMPU6050_Info.T = T;
					gMPU6050_Info.flag = 1;
					break;
				case 0x52:
					w[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*2000;	//X轴角速度
					w[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*2000;	//Y轴角速度
					w[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*2000;	//Z轴角速度
					T = ((short)(mTempBuf[9] << 8 | mTempBuf[8])) / 340.0+ 36.25;
					gMPU6050_Info.wx = w[0];
					gMPU6050_Info.wy = w[1];
					gMPU6050_Info.wz = w[2];
					gMPU6050_Info.T = T;
					gMPU6050_Info.flag = 1;
					break;
				case 0x53:
					angle[0] = ((short)(mTempBuf[3] << 8 | mTempBuf[2])) / 32768.0*180;	//X轴滚转角
					angle[1] = ((short)(mTempBuf[5] << 8 | mTempBuf[4])) / 32768.0*180;	//Y轴俯仰角
					angle[2] = ((short)(mTempBuf[7] << 8 | mTempBuf[6])) / 32768.0*180;	//Z轴航偏角
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
