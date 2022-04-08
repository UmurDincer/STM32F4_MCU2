/*
 * main.c
 *
 *  Created on: Mar 3, 2022
 *      Author: umurd
 */

#include"stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>


void UART2_Init(void);
void Error_handler(void);
void SystemClock_ConfigHSE(uint8_t clock_freq);
void GPIO_Init(void);
void TIMER6_Init(void);

TIM_HandleTypeDef htimer6;
UART_HandleTypeDef huart2;
extern uint8_t some_data[];

int main()
{
	HAL_Init();
	UART2_Init();
	TIMER6_Init();
	GPIO_Init();

	SCB->SCR |= ( 1 << 1);	// or we can use void HAL_PWR_EnableSleepOnExit(void) which is in pwr library

	TIM6->SR = 0; //to start with fresh STATUS REGISTER of TIMER to avoid any spurious interrupt

	//start the timer in interrupt mode
	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);
}


void GPIO_Init(void)
{
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	led.Pull = GPIO_NOPULL;
	led.Speed = GPIO_SPEED_FREQ_HIGH;

	__HAL_RCC_GPIOD_CLK_ENABLE();
	HAL_GPIO_Init(GPIOD, &led);

	button.Mode = GPIO_MODE_IT_FALLING;
	button.Pin = GPIO_PIN_0;
	button.Pull = GPIO_NOPULL;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &button);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if(HAL_UART_Init(&huart2) != HAL_OK)
	{
		//if there is a problem
		Error_handler();
	}


}
void TIMER6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 4999;
	htimer6.Init.Period = 32-1; // to set 10ms timer. (we use 16MHz HSI)
	if( HAL_TIM_Base_Init(&htimer6) != HAL_OK )
	{
		Error_handler();
	}

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY) != HAL_OK)
	{
		Error_handler();
	}

}

void Error_handler(void)
{
	while(1);
}
