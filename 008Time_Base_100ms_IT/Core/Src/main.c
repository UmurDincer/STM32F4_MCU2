/*
 * main.c
 *
 *  Created on: Feb 23, 2022
 *      Author: umurd
 */


#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

void Error_handler(void);
void SystemClock_Config(void);
void TIMER6_Init(void);
void GPIO_Init(void);

TIM_HandleTypeDef htimer6;

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
	TIMER6_Init();

	//Start Timer in IT mode
	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

}


void SystemClock_Config(void)
{

}

void GPIO_Init(void)
{
	GPIO_InitTypeDef led;

	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pin = GPIO_PIN_12;
	led.Pull = GPIO_NOPULL;
	led.Speed = GPIO_SPEED_FREQ_HIGH;

	__HAL_RCC_GPIOD_CLK_ENABLE();
	HAL_GPIO_Init(GPIOD, &led);

}

void TIMER6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 24;
	htimer6.Init.Period = 64000 - 1;
	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);

}

void Error_handler(void)
{
	while(1);
}
