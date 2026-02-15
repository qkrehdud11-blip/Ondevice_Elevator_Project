/*
 * delay.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */



#include "delay.h"

void delay_us(uint16_t us)
{
  uint16_t start = __HAL_TIM_GET_COUNTER(&htim11);
  while((__HAL_TIM_GET_COUNTER(&htim11) - start) < us);
}
