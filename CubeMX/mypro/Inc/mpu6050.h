#ifndef __mpu6050_H
#define __mpu6050_H

#include "main.h"
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	uint8_t flag;
	float T;
	float ax;
	float ay;
	float az;
	float wx;
	float wy;
	float wz;
	float anglex;
	float angley;
	float anglez;
}MPU6050_Info_t;

extern MPU6050_Info_t gMPU6050_Info;
extern float a[3];
extern float w[3];
extern float angle[3];
extern float T;

extern uint8_t Re_buf[11];
extern uint8_t temp_buf[11];
extern uint8_t counter;
extern uint8_t sign;

void MPU6050_Init(void);
void MPU6050_PraseData(void);

void HAL_MPU6050_RxCpltCallback(UART_HandleTypeDef *huart);
	
	
#ifdef __cplusplus
}
#endif

#endif
