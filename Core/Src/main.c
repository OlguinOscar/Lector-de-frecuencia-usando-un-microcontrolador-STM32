/*
 * main.c
 *
 *  Created on: May 18, 2024
 *      Author: osbit
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

void UART2_Init(void);
void Button(void);
void TIMER2_Init(void);
void GPIO_LED_ROJO(void);
void LSE_Configuration(void);

UART_HandleTypeDef uart1;
TIM_HandleTypeDef htim2;
TIM_IC_InitTypeDef timer2IC_Config;
GPIO_InitTypeDef button;
GPIO_InitTypeDef led_rojo;

const uint8_t true = 1;
const uint8_t false = 0;

uint32_t input_captures[2];
uint32_t capture_difference = 0;
uint8_t count = 1;
uint8_t is_captured = false;

double timer2_cnt_freq = 0;
double timer2_cnt_res = 0;
double user_signal_time_period = 0;
double user_signal_time_freq = 0;
char message[100] = "hola mundo\n\r";

RCC_OscInitTypeDef osc_init;
RCC_ClkInitTypeDef clk_init;

void delay(){
	for(int i = 0;i<400000;i++){
//		printf("hola mundo\n");
//		fflush(stdout);
	}
}

int main(void){

	HAL_Init();
	char message[100];
	TIMER2_Init();
	GPIO_LED_ROJO();

	UART2_Init();

	//HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);
	LSE_Configuration();

	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	while(1){
		if(is_captured){
			if(input_captures[1]>input_captures[0]){
				capture_difference = input_captures[1]-input_captures[0];
			}
			else{
				capture_difference = (0xFFFFFFFF-input_captures[0])+input_captures[1]; /// la primera resta calcula el tiempo antes del desbordamineto, y luego le suma lo que pasa despues del desbordamiento
			}
			timer2_cnt_freq = (HAL_RCC_GetPCLK1Freq())/(htim2.Init.Prescaler+1);
			timer2_cnt_res = 1/ timer2_cnt_freq;
			user_signal_time_period = capture_difference*timer2_cnt_res;
			user_signal_time_freq = (1/user_signal_time_period)/1000;
			sprintf(message, "%f kHz\r\n0",user_signal_time_freq);
			while(HAL_UART_Transmit(&uart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY)!= HAL_OK);
			is_captured = false;
		}

	}

	return 0;
}


void UART2_Init(void){
	uart1.Instance = USART6;
	uart1.Init.BaudRate = 9600;
	uart1.Init.WordLength = UART_WORDLENGTH_8B;
	uart1.Init.StopBits = UART_STOPBITS_1;
	uart1.Init.Parity = UART_PARITY_NONE;
	uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart1.Init.Mode = UART_MODE_TX_RX;
	if(HAL_UART_Init(&uart1)!= HAL_OK){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		while(1);
	}

}

void Button(void){
	button.Pin = GPIO_PIN_1;
	button.Mode = GPIO_MODE_INPUT;
	button.Speed = GPIO_SPEED_FREQ_LOW;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_Init(GPIOB, &button);
}

void TIMER2_Init(void){
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_IC_Init(&htim2)!= HAL_OK){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		while(1);
	}
	timer2IC_Config.ICFilter = 0; /// sin filtro;
	timer2IC_Config.ICPolarity = TIM_ICPOLARITY_RISING; /// DETECTA EN RISING EDGE
	timer2IC_Config.ICPrescaler = TIM_ICPSC_DIV1; /// PRESCALER EN EL TIM
	timer2IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI; /// SELECCIONAR LA ENTRADA DIRECTA
	if(HAL_TIM_IC_ConfigChannel(&htim2, &timer2IC_Config, TIM_CHANNEL_1)!= HAL_OK){ /// CONFIGURA EL CHANNEL 1 CON TOD/O LO ANTERIOR
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		while(1);
	}
}

void GPIO_LED_ROJO(void){
	led_rojo.Pin = GPIO_PIN_2;
	led_rojo.Mode = GPIO_MODE_OUTPUT_PP;
	led_rojo.Speed = GPIO_SPEED_FREQ_LOW;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_Init(GPIOB, &led_rojo);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){ /// se llama cuando pasa un rising en la señal (un capture)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	if(!is_captured){
		if(count == 1){
			input_captures[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count++;
		}
		else if(count==2){
			input_captures[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count = 1;
			is_captured = true;
		}
	}

}

void LSE_Configuration(void){
	RCC_OscInitTypeDef Osc_init;
	Osc_init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	Osc_init.LSEState = RCC_LSE_ON;
	if(HAL_RCC_OscConfig(&Osc_init)!= HAL_OK){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		while(1);
	}
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
}


