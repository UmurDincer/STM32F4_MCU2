/*
 * main.c
 *
 *  Created on: Mar 1, 2022
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
void CAN_Init(void);
void CAN1_Tx(void);
void CAN1_Rx(void);
void CAN_Filter_Config(void);
void TIMER6_Init(void);
void send_respose(uint32_t StdId);
void led_manage_output(uint8_t led_no);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;
TIM_HandleTypeDef htimer6;
CAN_RxHeaderTypeDef can1_rx;

uint8_t req_counter = 0;

int main(void)
{
	HAL_Init();
	SystemClock_ConfigHSE(SYS_CLOCK_FREQ_50_MHZ);
	GPIO_Init();
	UART2_Init();
	TIMER6_Init();
	CAN_Init();
	CAN_Filter_Config();

	if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_handler();
	}

	if(HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_handler();
	}

	CAN1_Tx();


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

void CAN_Init(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	//Setting related CAN bit timing
	hcan1.Init.Prescaler = 5;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

	if( HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_handler();
	}

}

void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef can1_filter;

	can1_filter.FilterActivation = ENABLE;
	can1_filter.FilterBank = 0;
	can1_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	can1_filter.FilterIdHigh = 0x0000;
	can1_filter.FilterIdLow = 0x0000;
	can1_filter.FilterMaskIdHigh = 0x0000;
	can1_filter.FilterMaskIdLow = 0x0000;
	can1_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can1_filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if(HAL_CAN_ConfigFilter(&hcan1, &can1_filter) != HAL_OK)
	{
		Error_handler();
	}
}

uint8_t led_no = 0;

void CAN1_Tx(void)
{
	CAN_TxHeaderTypeDef can1_tx;

	uint32_t TxMailBox;

	uint8_t message;

	can1_tx.DLC = 1;
	can1_tx.StdId = 0x65D;
	can1_tx.IDE = CAN_ID_STD;
	can1_tx.RTR = CAN_RTR_DATA;

	message = ++led_no;

	if(led_no == 4)
	{
		led_no = 0;
	}

	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);

	if(HAL_CAN_AddTxMessage(&hcan1, &can1_tx, &message , &TxMailBox) != HAL_OK)
	{
		Error_handler();
	}




}


/*
void CAN1_Rx(void)

{
	CAN_RxHeaderTypeDef can1_rx;
	uint8_t received[5];

	char notification[50];

	while(!HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0));

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can1_rx, received) != HAL_OK)
	{
		Error_handler();
	}

	sprintf(notification, "Message Reveived: %s\r\n", received);
	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);
}		We dont need Rx in this app. it is read by HAL_CAN_RxFifo0MsgPendingCallback
*/


void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char notification[50];

	sprintf(notification, "Message Transmitted:Mailbox0\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);

}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char notification[50];

	sprintf(notification, "Message Transmitted:Mailbox1\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);

}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	char notification[50];

	sprintf(notification, "Message Transmitted:Mailbox2\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	uint8_t received[8];
 	char notification[50];

	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can1_rx, received) != HAL_OK)
	{
		Error_handler();
	}

	if(can1_rx.StdId == 0x65D && can1_rx.RTR == 0)
	{
		// this is data frame sent by n1 to n2
		led_manage_output(received[0]);
		sprintf(notification, "Reply Received: #%X\r\n", received[0]);
	}
	else if(can1_rx.StdId == 0x651 && can1_rx.RTR == 1)
	{
		// this is remote frame sent by n1 to n2
		send_respose(can1_rx.StdId);
		return;
	}
	else if(can1_rx.StdId == 0x651 && can1_rx.RTR == 0)
	{
		//it is a reply (data frame) by n2 to n1
		sprintf(notification, "Reply Received: %#X\r\n", received[0] << 8 | received[1]);
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	char notification[50];

	sprintf(notification, "CAN Error Detected!\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)notification, strlen(notification), HAL_MAX_DELAY);

	Error_handler();
}

void TIMER6_Init(void)
{
	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 4999;
	htimer6.Init.Period = 10000-1;
	if( HAL_TIM_Base_Init(&htimer6) != HAL_OK )
	{
		Error_handler();
	}

}

void send_respose(uint32_t StdId)
{
	CAN_TxHeaderTypeDef header;
	uint32_t TxMailBox;
	uint8_t response[2] = {0xAB, 0xCD};

	header.DLC = 2;
	header.StdId = StdId;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;

	if(HAL_CAN_AddTxMessage(&hcan1, &header, response , &TxMailBox) != HAL_OK)
	{
		Error_handler();
	}
}
void led_manage_output(uint8_t led_no)
{
	switch(led_no)
	{
	case 1:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 CAN_TxHeaderTypeDef TxHeader;

	 uint32_t TxMailbox;

	 uint8_t message; //no meaning for data frame

	if ( req_counter  == 4)
	{
		//N1 sending Remote frame to N2
		TxHeader.DLC = 2; //N1 demanding 2 bytes of reply
		TxHeader.StdId = 0x651;
		TxHeader.IDE   = CAN_ID_STD;
		TxHeader.RTR = CAN_RTR_REMOTE;

		if( HAL_CAN_AddTxMessage(&hcan1,&TxHeader,&message,&TxMailbox) != HAL_OK)
		{
			Error_handler();
		}
		req_counter = 0;

	}else
	{
		CAN1_Tx();
		req_counter++;
	}

}


void Error_handler(void)
{
	while(1);
}

