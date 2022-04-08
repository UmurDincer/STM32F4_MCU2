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
//void SystemClock_ConfigHSE(uint8_t clock_freq);
void GPIO_Init(void);
//void TIMER6_Init(void);
void GPIO_AnalogConfig(void);

TIM_HandleTypeDef htimer6;
UART_HandleTypeDef huart2;
extern uint8_t some_data[];

char message[50];

int main()
{
	HAL_Init();
	UART2_Init();
	GPIO_Init();
	GPIO_AnalogConfig(); //set unused pins of configured GPIO as analog pin to save power.

	while(1)
	{

		if(HAL_UART_Transmit(&huart2, (uint8_t *)some_data, (uint16_t)strlen((char*)some_data), HAL_MAX_DELAY) != HAL_OK)
		{
			Error_handler();
		}
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_14, GPIO_PIN_SET);
		HAL_Delay(50);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_14, GPIO_PIN_RESET);

		memset(message, 0, sizeof(message));

		sprintf(message, "Going to sleep! \r\n");
		if(HAL_UART_Transmit(&huart2, (uint8_t *)message, (uint16_t)strlen((char*)message), HAL_MAX_DELAY) != HAL_OK)
		{
			Error_handler();
		}

		/*Systic is not required so disabled it before going to sleep*/
		HAL_SuspendTick();
		//going to sleep
		__WFE();
		__WFE(); // used __WFE instruction second times because of race condition.
				//  systick interrupt is fired just before the instruction executed so WFE is immediately excited and wakes up again.
		//MCU resumes here when it wakes up

		/*Enable the Systick*/
		HAL_ResumeTick();
		sprintf(message, "Woke up! \r\n");
		if(HAL_UART_Transmit(&huart2, (uint8_t *)message, (uint16_t)strlen((char*)message), HAL_MAX_DELAY) != HAL_OK)
		{
			Error_handler();
		}

	}
}


void GPIO_Init(void)
{
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	__HAL_RCC_GPIOD_CLK_SLEEP_DISABLE();

	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	led.Pull = GPIO_NOPULL;
	led.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOD, &led);

	button.Mode = GPIO_MODE_EVT_FALLING;
	button.Pin = GPIO_PIN_0;
	button.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &button);

#if 0
	HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
#endif

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
}

/*
 *
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

*/


/*
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
*/

void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 460800; // increasing baudrate decreases power consumption
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


/*
void TIMER6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 4999;
	htimer6.Init.Period = 100-1;
	if( HAL_TIM_Base_Init(&htimer6) != HAL_OK )
	{
		Error_handler();
	}

}
*/

void GPIO_AnalogConfig(void)
{
	GPIO_InitTypeDef gpio_analog1, gpio_analog2;
	uint32_t gpio_pins = GPIO_PIN_1 | GPIO_PIN_4 /
						 GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 /
						 GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 /
						 GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 /
						 GPIO_PIN_14 | GPIO_PIN_15;

	gpio_analog1.Pin = gpio_pins;
	gpio_analog1.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &gpio_analog1);

	gpio_pins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 /
				GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 /
				GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 /
				GPIO_PIN_11 | GPIO_PIN_2| GPIO_PIN_3;


	gpio_analog2.Pin = gpio_pins;
	gpio_analog2.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOD, &gpio_analog2);


}

void Error_handler(void)
{
	while(1);
}
