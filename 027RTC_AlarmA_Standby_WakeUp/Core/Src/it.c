/*
 * it.c
 *
 *  Created on: Mar 5, 2022
 *      Author: umurd
 */


#include "main.h"
extern RTC_HandleTypeDef hrtc;

void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI0_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void RTC_Alarm_IRQHandler(void)
{
	HAL_RTC_AlarmIRQHandler(&hrtc);
}
