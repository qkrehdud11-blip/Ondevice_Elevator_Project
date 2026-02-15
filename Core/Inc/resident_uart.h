/*
 * resident_uart.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 *
 *
 *   - 거주자(UART) 콘솔 모듈
 *  - UART로 명령을 입력받아 엘리베이터 상태 조회/호출 등을 수행
 *  - 논블로킹: 수신은 인터럽트, 처리/자동출력은 Task에서 수행
 */

#ifndef INC_RESIDENT_UART_H_
#define INC_RESIDENT_UART_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>


/**
 * @brief  거주자 UART 초기화 (예: USART2)
 * @param  huart : 사용할 UART 핸들 포인터
 */
void ResidentUART_Init(UART_HandleTypeDef *huart);


/**
 * @brief  주기적으로 호출하는 Task
 *         (층 상태 변화 시 자동 출력 등)
 */
void ResidentUART_Task(void);




#endif /* INC_RESIDENT_UART_H_ */
