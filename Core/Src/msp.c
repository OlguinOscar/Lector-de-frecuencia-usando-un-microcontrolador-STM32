/*
 * msp.c
 *
 *  Created on: May 18, 2024
 *      Author: osbit
 */

#include "stm32f4xx_hal.h"

void HAL_MspInit(void){
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); /// aqui basicamente le dices al NVIC como quieres que gestione las interrupciones, y como es 4, predomina la preemcion, que significa que si ocurre una interrupcion y luego otra se presenta de manera inesperada, la primera la ignora y realiza la segunda si es tiene mayor prioridad
	SCB->SHCSR |= (0x7<<16);//usage fault, memory fault y bus fault activados

	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0); // memory fault
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0); // bus fault
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0); //usage fault

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef gpio_uart1;
	__HAL_RCC_USART6_CLK_ENABLE(); //reloj uart1/usart1
	__HAL_RCC_GPIOA_CLK_ENABLE();

	gpio_uart1.Pin = GPIO_PIN_11; //tx
	gpio_uart1.Mode = GPIO_MODE_AF_PP;
	gpio_uart1.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart1.Pull = GPIO_PULLUP;
	gpio_uart1.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOA, &gpio_uart1);

	gpio_uart1.Pin = GPIO_PIN_12;//rx
	HAL_GPIO_Init(GPIOA, &gpio_uart1);

	//HAL_NVIC_EnableIRQ(USART2_IRQn); // por si quieres utilizar interrupciones con USART2
	//HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);// setear la prioridad de la interrupcion 15 solo por poner algo(no importa), el otro es cero porque la subprioridad es como un dont care

}

//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim){
	///CLOCK TIM11
///	__HAL_RCC_TIM11_CLK_ENABLE();
///	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
//	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 15, 0);/// LO MISMO QUE ARRIBA XD
//}  INICIALIZA EL TIM 11, LO COMENTE PORQUE VOY A USAR EL QUE ESTA ABAJO

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim){ /// ESTE ES PARA INICIARLO NO EN EL MODO DE ARRIBA, SINO CUANDO QUIERES UTILIZAR LOS CANALES DE ENTRADA Y SALIDA
	GPIO_InitTypeDef tim2ch1_gpio;
	__HAL_RCC_TIM2_CLK_ENABLE(); // INICIA EL RELOJ DEL TIM2
	__HAL_RCC_GPIOA_CLK_ENABLE();
	tim2ch1_gpio.Pin = GPIO_PIN_0; //a0 channel 1
	tim2ch1_gpio.Mode = GPIO_MODE_AF_PP;
	tim2ch1_gpio.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &tim2ch1_gpio);

	//nvic configuration
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

}




