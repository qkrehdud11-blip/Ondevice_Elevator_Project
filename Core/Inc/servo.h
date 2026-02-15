/*
 * servo.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 *
 *
 *  서보모터 제어 모듈 (엘리베이터 문 제어용)
 *  - PWM(TIM1 CH1) 기반 제어
 *  - 논블로킹 방식 (Servo_Run을 주기적으로 호출)
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_





#include "stm32f4xx_hal.h"
#include <stdbool.h>




/* ==============================
 *   서보 위치 값 (CCR1 값 기준)
 * ==============================
 * TIM1->CCR1 값으로 서보 각도 제어
 *
 * SERVO_LEFT  : 문 닫힘 위치
 * SERVO_RIGHT : 문 열림 위치
 *
 * ※ PWM 주기 설정에 따라 값은 조정 가능
 */
#define SERVO_LEFT   40    // 닫힘
#define SERVO_RIGHT  240   // 열림



/* ==============================
 *       서보 동작 상태
 * ============================== */
typedef enum
{
    SERVO_STOP,
    SERVO_MOVING
} SERVO_STATE;



/* ==============================
 *         API 함수
 * ============================== */
void Servo_Init(void);     // 초기화 (기본 닫힘 상태)
void Servo_Open(void);     // 문 열기
void Servo_Close(void);    // 문 닫기
void Servo_Run(void);      // 논블로킹 이동 처리 (주기 호출)

bool Servo_IsOpened(void); // 완전 열림 여부
bool Servo_IsClosed(void); // 완전 닫힘 여부




#endif /* INC_SERVO_H_ */
