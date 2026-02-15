/*
 * servo.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 *
  *  엘리베이터 문 제어용 서보 모터 제어
 *  - TIM1 CH1 PWM 사용
 *  - HAL_GetTick() 기반 논블로킹 이동
 */



/* servo.c */
#include "servo.h"
#include "tim.h"

/* ==============================
 *        내부 상태 변수
 * ============================== */
static SERVO_STATE servoState;   // 현재 상태
static uint16_t currentPosition; // 현재 CCR1 값
static uint16_t targetPosition;  // 목표 CCR1 값
static uint32_t prevTime;        // 마지막 이동 시각(ms)



/* ==============================
 *        초기화
 * ============================== */
void Servo_Init(void)
{
    servoState = SERVO_STOP;
    currentPosition = SERVO_LEFT;   // 기본 닫힘(왼쪽)
    targetPosition  = SERVO_LEFT;
    prevTime = HAL_GetTick();

    // PWM 출력 초기 위치 적용
    TIM1->CCR1 = currentPosition;
}


/* ==============================
 *        문 열기 요청
 * ============================== */
void Servo_Open(void)
{
    targetPosition = SERVO_RIGHT;	// 열림 위치 설정
    servoState = SERVO_MOVING;		// 이동 시작
}


/* ==============================
 *        문 닫기 요청
 * ============================== */
void Servo_Close(void)
{
    targetPosition = SERVO_LEFT;	// 닫힘 위치 설정
    servoState = SERVO_MOVING;		// 이동 시작
}


/* ==============================
 *     논블로킹 이동 처리 함수
 * ==============================
 * main loop에서 계속 호출해야 함
 */
void Servo_Run(void)
{
    if (servoState == SERVO_STOP)
        return;   // 이미 목표 도달

    // 20ms마다 1단계씩 이동
    if (HAL_GetTick() - prevTime < 20)
        return;

    prevTime = HAL_GetTick();

    // 목표 위치까지 1씩 증가/감소
    if (currentPosition < targetPosition)
        currentPosition++;
    else if (currentPosition > targetPosition)
        currentPosition--;
    else
        servoState = SERVO_STOP;  // 목표 도달

    // PWM 레지스터 갱신
    TIM1->CCR1 = currentPosition;
}


/* ==============================
 *       상태 확인 함수
 * ============================== */
bool Servo_IsOpened(void) { return (currentPosition == SERVO_RIGHT); }
bool Servo_IsClosed(void) { return (currentPosition == SERVO_LEFT);  }







