/*
 * msp.c
 *
 *  Created on: Feb 23, 2022
 *      Author: umurd
 */

#include "main.h"

void HAL_MspInit(void)
{
	// low level processor specific initialization
	//1. Set up the priority groupingf of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	//2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= (0x7 << 16); // 0x7 = 111 so 16. 17. and 18. bits are enabled
							   // which are MEMFAULTENA, BUSFAULTENA, ,USGFAULTENA

	//3. Configure the priority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef tim2_ch1;

	//1. Enable the clock for timer 2
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//2. Configure a GPIO to behave as timer2 channel
	tim2_ch1.Pin = GPIO_PIN_0;
	tim2_ch1.Mode = GPIO_MODE_AF_PP;
	tim2_ch1.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &tim2_ch1);

	//3. nvic settings
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);



}
