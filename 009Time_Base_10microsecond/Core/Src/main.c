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
void SystemClock_Config(uint8_t clock_freq);
void TIMER6_Init(void);
void GPIO_Init(void);

TIM_HandleTypeDef htimer6;

int main(void)
{
	HAL_Init();
	SystemClock_Config(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	TIMER6_Init();


	//Start Timer
	HAL_TIM_Base_Start(&htimer6);

	while(1)
	{
		//Loop until the update event flag is set
		while(!(TIM6->SR & TIM_SR_UIF));
		/*
		 * The required time delay has been elapsed
		 * User code can be executed.
		 */
		TIM6->SR = 0;
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	}


}


void SystemClock_Config(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint8_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSICalibrationValue = 16;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;


	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 100;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

		FLatency = FLASH_ACR_LATENCY_1WS;
		break;

	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 168;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		FLatency = FLASH_ACR_LATENCY_2WS;
		break;

	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM = 16;
		osc_init.PLL.PLLN = 240;
		osc_init.PLL.PLLP = 2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		FLatency = FLASH_ACR_LATENCY_3WS;
		break;

	}
	default: return;
	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_RCC_ClockConfig(&clk_init,FLatency ))
	{
		Error_handler();
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
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
	htimer6.Init.Prescaler = 9;
	htimer6.Init.Period = 50 - 1;
	if(HAL_TIM_Base_Init(&htimer6) != HAL_OK)
	{
		Error_handler();
	}

}

void Error_handler(void)
{
	while(1);
}
