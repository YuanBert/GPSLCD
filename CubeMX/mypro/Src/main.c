
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdlib.h"
#include "mpu6050.h"
#include "gps.h"
#include "lcd.h"
#include "math.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static uint8_t MPU6050Buffer[30];
static uint8_t MPU6050Buffer1[30];
static uint8_t ClearBuffer[30] = {0};
uint16_t gTim3Cnt;
uint8_t gTim3Flag;
uint8_t gTim3UpdataFlag;

uint8_t gInitFlag;
uint8_t gNGPSflag;
float gZangle;
float gSpeed;
float gAngle;
double gDistance;
float gInslongitude;
float gInslatitude;
double gEarthRadio;

uint8_t UTC[30];
uint8_t LatLongInfo[30];
uint8_t PositingInfo[30];
uint8_t GroundSpeedInfo[30];
uint8_t GroundCourseinfo[30];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
float Du2Fen(float data)
{
	float re;
	uint16_t du;
	uint16_t fen;
	float miao;
	du = (uint16_t)data;
	fen = (uint16_t)((data - du) * 60.0);
	miao = (data - du) * 60.0 - fen;
	re = du * 1000 + fen + miao;
	return re;
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t data = 0x52;
	float angle; 
	gEarthRadio = 6371393.0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM5_Init();
  MX_TIM3_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  gInitFlag = 0;
  LCD_Init();
  LED_Init();
  MPU6050_Init();
  HAL_TIM_Base_Start_IT(&htim3);
  //HAL_TIM_Base_Start_IT(&htim5);
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
  
  POINT_COLOR=RED; 
  LCD_Clear(BLUE);
  LCD_Display_Dir(1);
  
	LCD_ShowString(40,40,500,12,12,(uint8_t*)"Horizontal placement,calibration...");  
	HAL_Delay(4500);
	HAL_UART_Transmit(&huart2,&data,1,0xFFFF);
	LCD_Clear(BLUE);
	LCD_ShowString(40,40,500,12,12,(uint8_t*)"calibration OK..."); 
	HAL_Delay(500);
	POINT_COLOR=RED; 
	LCD_Clear(BLUE);
	LCD_Display_Dir(1);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  GPS_dataPrase();
	  MPU6050_PraseData();

	  
	  if(1 == gTim3Flag)
	  {
		gTim3Flag = 0;
		  
		  if(1 == gMPU6050_Info.flag)
		  {
			  sprintf((char*)MPU6050Buffer,"ax=%.2f ay=%.2f az=%.2f",gMPU6050_Info.ax,gMPU6050_Info.ay,gMPU6050_Info.az);
			  sprintf((char*)MPU6050Buffer1,"Ax=%.2f Ay=%.2f Az=%.2f",gMPU6050_Info.anglex,gMPU6050_Info.angley,gMPU6050_Info.anglez);
			  
			  LCD_ShowString(20,150,360,12,12,ClearBuffer);  
			  LCD_ShowString(20,170,360,12,12,ClearBuffer);
			  
			  LCD_ShowString(40,40,200,24,24,(uint8_t*)"Navigation System");  
			  LCD_ShowString(20,150,360,12,12,MPU6050Buffer);  
			  LCD_ShowString(20,170,360,12,12,MPU6050Buffer1); 
			  memset(MPU6050Buffer, 0, 30);
			  memset(MPU6050Buffer1,0, 30);
			  gMPU6050_Info.flag = 0;
		  }
		LED0=!LED0;
	  }
		  /* ���Ե�����GPS�źŶ�ʧ֮��ĽǶ� - 1s */
	  if(gTim3UpdataFlag)
	  {
		gTim3UpdataFlag = 0;
		if(1 == gNGPSflag && 1 == gInitFlag)
		{
			gDistance += (gSpeed*0.514 + 4.9 * a[1]);
			
			//angle = Du2Fen(gZangle);
			gInslongitude = (sin(angle/3.1415) * gDistance)/gEarthRadio;
			gInslongitude = Du2Fen(gInslongitude) + atof(Save_Data.longitude);
			
			gInslatitude = atof(Save_Data.latitude);
			gInslatitude =  gInslatitude + Du2Fen(gMPU6050_Info.anglez - gZangle);
		
			sprintf((char *)LatLongInfo,"%.3f:%s %.3f:%s",gInslatitude,Save_Data.E_W,gInslongitude,Save_Data.N_S);
			LCD_ShowString(30,110,300,16,16,LatLongInfo);
		}
		else
		{
			gDistance = 0;
			gInslongitude = 0;
			gInslatitude = 0;
			
		}			
	  }
	  
	  if(GPSDataFlag)
	  {
		POINT_COLOR=RED; 
		LCD_Display_Dir(1);
		  
		LCD_ShowString(5,5,360,12,12,(uint8_t*)ClearBuffer);//��ʾUTCʱ����Ϣ
		LCD_ShowString(30,90,300,16,16,ClearBuffer);
		LCD_ShowString(30,110,300,16,16,ClearBuffer);
		LCD_ShowString(30,130,300,16,16,ClearBuffer);
		LCD_ShowString(40,40,200,24,24,(uint8_t*)"Navigation System");  
		LCD_ShowString(5,5,360,12,12,(uint8_t*)UTC);//��ʾUTCʱ����Ϣ
		LCD_ShowString(30,90,300,16,16,PositingInfo);
		LCD_ShowString(30,110,300,16,16,LatLongInfo);
		LCD_ShowString(30,130,300,16,16,GroundSpeedInfo);
		memset(UTC,0,30);
		memset(PositingInfo,0,30);
		memset(LatLongInfo,0,30);
		memset(GroundCourseinfo,0,30);
		GPSDataFlag = 0;
	  }
	   
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* TIM5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Period elapsed callback in non blocking mode 
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
	if(htim->Instance == htim5.Instance)
	{
		timeCnt++;
		
	}
	
	if(htim->Instance == htim3.Instance)
	{
		gTim3Flag = 1;
		
		gTim3Cnt++;
		if(gTim3Cnt > 9)
		{
			gTim3UpdataFlag = 1;
			gTim3Cnt = 0;
		}
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
