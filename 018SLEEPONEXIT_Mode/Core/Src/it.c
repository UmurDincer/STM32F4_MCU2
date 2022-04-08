/*
 * it.c
 *
 *  Created on: Mar 3, 2022
 *      Author: umurd
 */

#include"main.h"

extern TIM_HandleTypeDef htimer6;
extern UART_HandleTypeDef huart2;
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


void TIM6_DAC_IRQHandler(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htimer6);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
}

void EXTI0_IRQHandler(void)
{
	HAL_TIM_Base_Start_IT(&htimer6);
 }
