/*
 * main.c
 *
 *  Created on: Feb 28, 2022
 *      Author: umurd
 */



#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>



void UART2_Init(void);
void Error_handler(void);
void SystemClock_ConfigHSE(uint8_t clock_freq);
void GPIO_Init(void);
void TIMER2_Init(void);


UART_HandleTypeDef huart2;
TIM_HandleTypeDef htimer2;


int main(void)
{

	HAL_Init();
	SystemClock_ConfigHSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	UART2_Init();
	TIMER2_Init();

	if(HAL_TIM_PWM_Start(&htimer2,TIM_CHANNEL_1) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_TIM_PWM_Start(&htimer2,TIM_CHANNEL_2) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_TIM_PWM_Start(&htimer2,TIM_CHANNEL_3) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_TIM_PWM_Start(&htimer2,TIM_CHANNEL_4) != HAL_OK)
	{
		Error_handler();
	}

	while(1);



}


void SystemClock_ConfigHSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	uint8_t FLatency = 0;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;


	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 8;
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
		osc_init.PLL.PLLM = 8;
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
		osc_init.PLL.PLLM = 8;
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

void TIMER2_Init(void)
{
	TIM_OC_InitTypeDef tim2PWM_config;
	// produce period of 1 second.
	htimer2.Instance = TIM2;
	htimer2.Init.Period = 10000 - 1 ;
	htimer2.Init.Prescaler = 4999;
	if( HAL_TIM_PWM_Init(&htimer2) != HAL_OK)
	{
		Error_handler();
	}

	tim2PWM_config.OCMode = TIM_OCMODE_PWM1;
	tim2PWM_config.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim2PWM_config.Pulse = (htimer2.Init.Period * 25) / 100; // we want 25% pwm so ((40/100) * (10000 - 1))
	if(HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2PWM_config, TIM_CHANNEL_1))
	{
		Error_handler();
	}

	tim2PWM_config.Pulse = (htimer2.Init.Period * 45) / 100; // we want 45% pwm so ((40/100) * (10000 - 1))
	if(HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2PWM_config, TIM_CHANNEL_2))
	{
		Error_handler();
	}

	tim2PWM_config.Pulse = (htimer2.Init.Period * 75) / 100; // we want 50% pwm so ((75/100) * (10000 - 1))
	if(HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2PWM_config, TIM_CHANNEL_3))
	{
		Error_handler();
	}

	tim2PWM_config.Pulse = (htimer2.Init.Period * 95) / 100; // we want 75% pwm so ((95/100) * (10000 - 1))
	if(HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2PWM_config, TIM_CHANNEL_4))
	{
		Error_handler();
	}

}


void Error_handler(void)
{
	while(1);
}

