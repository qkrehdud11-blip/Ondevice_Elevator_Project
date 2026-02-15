/*
 * logger.h
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_



#include "stm32f4xx_hal.h"
#include <stdarg.h>

/**
 * @brief  로그 출력에 사용할 UART 등록
 * @param  huart : 사용할 UART 핸들 (예: &huart2)
 */
void Log_Init(UART_HandleTypeDef *huart);

/**
 * @brief  printf 스타일 로그 출력
 * @param  fmt : printf 형식 문자열
 *
 * 내부적으로 vsnprintf → HAL_UART_Transmit 호출
 */
void Log_Printf(const char *fmt, ...);

#endif /* INC_LOGGER_H_ */
