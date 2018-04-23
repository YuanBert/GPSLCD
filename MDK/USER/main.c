#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
/************************************************
 ALIENTEK Mini STM32F103������ʵ��11
 TFTLCD��ʾʵ�� -HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{ 
	u8 x=0;
	u8 lcd_id[12];						//���LCD ID�ַ���
	
  HAL_Init();                    	 	//��ʼ��HAL��    
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					      //��ʼ������
	LED_Init();							          //��ʼ��LED	
 	LCD_Init();
	POINT_COLOR=RED; 
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//��LCD ID��ӡ��lcd_id���顣				 	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,40,200,24,24,"Mini STM32 ^_^");	
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
		LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
 		LCD_ShowString(30,110,200,16,16,lcd_id);		//��ʾLCD ID	      					 
		LCD_ShowString(30,130,200,12,12,"2017/6/2");	      					 
	    x++;
		if(x==12)x=0;
		LED0=!LED0;	 
		delay_ms(1000);	
	} 
}
