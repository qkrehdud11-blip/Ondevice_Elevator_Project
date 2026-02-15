/*
 * stepper.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 *
 *  STEP 모터(4상) 구동 모듈
 *  - HAL_GetTick() 기반 논블로킹 방식
 *  - Stepper_Task()를 주기적으로 호출하면 설정된 방향으로 계속 회전
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_




#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>




/*
 * ==============================
 *        회전 방향 정의
 * ==============================
 * DIR_UP   : 시계 방향 (엘리베이터 상승 방향)
 * DIR_DOWN : 반시계 방향 (엘리베이터 하강 방향)
 */
#define DIR_UP     0   // CW
#define DIR_DOWN   1   // CCW


/*
 * ==============================
 *     스텝모터 제어 핀 매핑
 * ==============================
 * 4상(HALF STEP) 제어용 GPIO 정의
 * 각 INx는 드라이버 입력 또는 ULN2003 입력과 연결
 */
#define IN1_PORT GPIOA
#define IN1_PIN  GPIO_PIN_4

#define IN2_PORT GPIOB
#define IN2_PIN  GPIO_PIN_0

#define IN3_PORT GPIOC
#define IN3_PIN  GPIO_PIN_1

#define IN4_PORT GPIOC
#define IN4_PIN  GPIO_PIN_0


/*
 * ==============================
 *        함수 원형 선언
 * ==============================
 */

/**
 * @brief  스텝모터 초기화
 *         내부 상태 변수 및 출력 핀 초기 설정
 */
void Stepper_Init(void);

/**
 * @brief  연속 회전 시작(논블로킹)
 * @param  dir : DIR_UP 또는 DIR_DOWN
 *
 * Stepper_Task()가 호출될 때마다 일정 주기(STEP_PERIOD_MS)로 한 스텝씩 진행
 */
void Stepper_StartContinuous(uint8_t dir);


/**
 * @brief  회전 정지
 *
 * 주의:
 * - 현재 구현은 phase 0 출력으로 "홀드(토크 유지)" 상태가 될 수 있음.
 * - 완전 OFF가 필요하면 모든 IN 핀을 RESET 하는 방식으로 변경해야 함.
 */
void Stepper_Stop(void);


/**
 * @brief  주기적 호출용 Task(논블로킹)
 *
 * main loop 또는 타이머 기반 task에서 자주 호출.
 * 내부에서 HAL_GetTick()으로 스텝 주기를 맞추며,
 * s_busy=true일 때만 실제 상(phase)을 갱신한다.
 */
void Stepper_Task(void);

/**
 * @brief  모터 동작 상태 확인
 * @retval true  : 회전 진행 중
 * @retval false : 정지 상태
 */
bool Stepper_IsBusy(void);


#endif /* INC_STEPPER_H_ */
