/*
 * it.c
 *
 *  Created on: Feb 28, 2022
 *      Author: umurd
 */


#include "main.h"

extern TIM_HandleTypeDef htimer2;

void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
