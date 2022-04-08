/*
 * main.c
 *
 *  Created on: Feb 23, 2022
 *      Author: umurd
 */


#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
void SystemClockConfig(void);
void UART2_Init(void);
void Error_handler(void);
uint8_t convert_to_capital(uint8_t data);

UART_HandleTypeDef huart2;

char *user_data = "The application is running\r\n";

int main(void)
{
	 HAL_Init();
	 SystemClockConfig();
	 UART2_Init();

	 if(HAL_UART_Transmit(&huart2, (uint8_t*)user_data, (uint16_t)strlen(user_data), HAL_MAX_DELAY) != HAL_OK)
	 {
		 Error_handler();
	 }

	 uint8_t received_data;
	 uint8_t data_buffer[100];
	 uint32_t count = 0;
	 while(1)
	 {
		 HAL_UART_Receive(&huart2, &received_data, 1, HAL_MAX_DELAY);
		 if(received_data == '\r')
		 {
			break;
		 }
		 else
		 {
			 data_buffer[count++] = convert_to_capital(received_data);
		 }
	 }

	 data_buffer[count++] = '\r';
	 if(HAL_UART_Transmit(&huart2, (uint8_t*)data_buffer,count, HAL_MAX_DELAY) != HAL_OK)
	 	 {
	 		 Error_handler();
	 	 }

}


uint8_t convert_to_capital(uint8_t data)
{
	if(data >= 'a' && data <= 'z')
	{
		data -= 32;
	}

	return data;
}

void SystemClockConfig(void)
{

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
