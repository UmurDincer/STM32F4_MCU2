/*
 * it.c
 *
 *  Created on: Feb 23, 2022
 *      Author: umurd
 */
#include "main.h"


void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}

