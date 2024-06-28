/*
 * it.c
 *
 *  Created on: May 18, 2024
 *      Author: osbit
 */


#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim2;

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}


void TIM2_IRQHandler(void){
	HAL_TIM_IRQHandler(&htim2);
}
