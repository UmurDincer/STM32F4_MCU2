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

UART_HandleTypeDef huart2;
uint32_t count = 0;

void UART2_Init(void);
void Error_handler(void);
void SystemClock_Config(uint8_t clock_freq);

char msg[100];
int main(void)
{
	uint8_t data_buffer[100];
	uint8_t received_data;
	uint8_t select = 0;
	HAL_Init();

	UART2_Init();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "Press number for\n[1]SYS_CLOCK_FREQ_50_MHZ\n[2]SYS_CLOCK_FREQ_84_MHZ\n[3]SYS_CLOCK_FREQ_120_MHZ\n\nEntry:");
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
	while(1)
	 {
		 HAL_UART_Receive(&huart2, &received_data, 1, HAL_MAX_DELAY);
		 if(received_data == '\r')
		 {
			break;
		 }
		 else
		 {
			 data_buffer[count++] = received_data;
		 }
	 }

	if(data_buffer[0] ==  '1')
	{
		select = SYS_CLOCK_FREQ_50_MHZ;
	}
	else if(data_buffer[0] ==  '2')
	{
		select = SYS_CLOCK_FREQ_84_MHZ;
	}
	else if(data_buffer[0] ==  '3')
	{
		select = SYS_CLOCK_FREQ_120_MHZ;
	}

	SystemClock_Config(select);

	UART2_Init();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SYSCLK: %ldHz\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK: %ldHz\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK1: %ldHz\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK2: %ldHz\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "-------------------------------------\r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	while(1);



}


void SystemClock_Config(uint8_t clock_freq)
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

void Error_handler(void)
{
	while(1);
}
