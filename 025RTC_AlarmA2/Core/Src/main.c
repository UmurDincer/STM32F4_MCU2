/*
 * main.c
 *
 *  Created on: Mar 5, 2022
 *      Author: umurd
 */


#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include<stdarg.h>


void UART2_Init(void);
void Error_handler(void);
void SystemClock_Config(uint8_t clock_freq);
void GPIO_Init(void);
void RTC_Init(void);
void RTC_CalendarConfig(void);
char* GetDayofWeek(uint8_t day);
void RTC_AlarmAConfig(void);

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htimer2;
RTC_HandleTypeDef hrtc;


char message[150];

void printmsg(char *format, ...)
{
	char str[80];

	va_list args;
	va_start(args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
}

int main(void)
{

	HAL_Init();

	SystemClock_Config(SYS_CLOCK_FREQ_50_MHZ);

	GPIO_Init();

	UART2_Init();

	RTC_Init();


	while(1);

}

void RTC_Init(void)
{
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_12;
	hrtc.Init.AsynchPrediv = 0x7F;
	hrtc.Init.SynchPrediv = 0xFF;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_handler();
	}

}

void RTC_CalendarConfig(void)
{
	RTC_TimeTypeDef RTC_Time_Init;
	RTC_DateTypeDef RTC_Date_Init;

	//this funtion does RTC Calendar Configurations
	//Lets configure the calendar for time 12:11:10 Date: 12 June 2018 Tuesday

	RTC_Time_Init.Hours = 12;
	RTC_Time_Init.Minutes = 0;
	RTC_Time_Init.Seconds = 0;
	RTC_Time_Init.TimeFormat = RTC_HOURFORMAT12_PM;
	if(HAL_RTC_SetTime(&hrtc, &RTC_Time_Init, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	RTC_Date_Init.Date = 12;
	RTC_Date_Init.Month = RTC_MONTH_JUNE;
	RTC_Date_Init.Year = 18;
	RTC_Date_Init.WeekDay = RTC_WEEKDAY_TUESDAY;
	if(HAL_RTC_SetDate(&hrtc, &RTC_Date_Init, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}
}

/*
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


void SystemClock_Config(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;



	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.LSIState = RCC_LSI_ON;
	osc_init.HSICalibrationValue = 16;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;


	switch(clock_freq)
	{
	case SYS_CLOCK_FREQ_50_MHZ:
	{
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 50;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

		break;

	}
	case SYS_CLOCK_FREQ_84_MHZ:
	{
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 84;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

		break;

	}
	case SYS_CLOCK_FREQ_120_MHZ:
	{
		osc_init.PLL.PLLM = 8;
		osc_init.PLL.PLLN = 120;
		osc_init.PLL.PLLP = RCC_PLLP_DIV2;
		osc_init.PLL.PLLQ = 2; //default value

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
							 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
		break;

	}
	default: return;
	}

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_RCC_ClockConfig(&clk_init,FLASH_LATENCY_2 ) != HAL_OK)
	{
		Error_handler();
	}
	uint32_t freq = HAL_RCC_GetHCLKFreq();
	HAL_SYSTICK_Config(freq/ 1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

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


	GPIO_InitTypeDef button;
	button.Mode = GPIO_MODE_IT_FALLING;
	button.Pin = GPIO_PIN_0;
	button.Pull = GPIO_NOPULL;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &button);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);


}

char* GetDayofWeek(uint8_t day)
{
	char* weekday[] = {" ","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

	return weekday[day];
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	RTC_TimeTypeDef RTC_TimeRead;
	RTC_DateTypeDef RTC_DateRead;

	RTC_CalendarConfig();

	if(HAL_RTC_GetTime(&hrtc, &RTC_TimeRead, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_RTC_GetDate(&hrtc, &RTC_DateRead, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	printmsg("Current Time: %02d:%02d:%02d\r\n", RTC_TimeRead.Hours,  RTC_TimeRead.Minutes, \
			RTC_TimeRead.Seconds);

	printmsg("Current Date: %2d/%2d/%2d	[%s]\r\n", RTC_DateRead.Month, RTC_DateRead.Date, \
			RTC_DateRead.Year, GetDayofWeek(RTC_DateRead.WeekDay));

	RTC_AlarmAConfig();

}


void RTC_AlarmAConfig(void)
{
	RTC_AlarmTypeDef AlarmA_Set;

	memset(&AlarmA_Set, 0, sizeof(AlarmA_Set));
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

	AlarmA_Set.Alarm = RTC_ALARM_A;
	AlarmA_Set.AlarmTime.Hours = 12;
	AlarmA_Set.AlarmTime.Minutes = 0;
	AlarmA_Set.AlarmTime.Seconds = 15;
	AlarmA_Set.AlarmTime.TimeFormat = RTC_HOURFORMAT12_PM;
	AlarmA_Set.AlarmMask= RTC_ALARMMASK_DATEWEEKDAY ;
	AlarmA_Set.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;


	//12:00:15
	if(HAL_RTC_SetAlarm_IT(&hrtc, &AlarmA_Set, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_handler();
	}

	printmsg("Alarm set successfully!\r\n");
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_TimeTypeDef RTC_TimeRead;

	HAL_RTC_GetTime(hrtc, &RTC_TimeRead, RTC_FORMAT_BIN);

	printmsg("Current Time: %02d:%02d:%02d\r\n", RTC_TimeRead.Hours,  RTC_TimeRead.Minutes, \
			RTC_TimeRead.Seconds);

	printmsg("Alarm Trigerred. \r\n");
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

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

