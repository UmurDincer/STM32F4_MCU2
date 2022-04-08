/*
 * it.c
 *
 *  Created on: Feb 23, 2022
 *      Author: umurd
 */


void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}
