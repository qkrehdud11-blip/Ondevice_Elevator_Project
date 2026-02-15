/*
 * stepper.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */




#include "stepper.h"



/* ==============================
 *   HALF STEP 출력 테이블
 * ==============================
 * 각 행(phase)은 IN1~IN4에 출력할 ON/OFF 상태를 의미한다.
 * 일반적인 28BYJ-48 + ULN2003 조합에서 많이 사용하는 half-step 패턴.
 */
static const uint8_t HALF_STEP_SEQ[8][4] =
{
		/* IN1 IN2 IN3 IN4 */
		{1,0,0,0},
		{1,1,0,0},
		{0,1,0,0},
		{0,1,1,0},
		{0,0,1,0},
		{0,0,1,1},
		{0,0,0,1},
		{1,0,0,1}
};


/* ==============================
 *        내부 상태 변수
 * ============================== */
static uint8_t  s_stepIndex = 0;			// 현재 phase(0~7)
static uint32_t s_prevTick  = 0;			// 마지막 스텝 갱신 시각(ms)
static volatile bool s_busy = false;		// 회전 동작 중 여부
static volatile uint8_t s_dir = DIR_UP;		// 방향(DIR_UP / DIR_DOWN)


/* 스텝 진행 주기(ms)
 * - 값이 작을수록 빠르게 회전하지만 토크 저하/탈조 가능성 증가
 * - 2ms는 꽤 빠른 편이라 부하/전원 상태에 따라 조정 필요
 */
#define STEP_PERIOD_MS  2


/* ==============================
 *   특정 phase를 GPIO에 출력
 * ============================== */
static void Stepper_WritePhase(uint8_t phase)
{
  HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, HALF_STEP_SEQ[phase][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN2_PORT, IN2_PIN, HALF_STEP_SEQ[phase][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN3_PORT, IN3_PIN, HALF_STEP_SEQ[phase][2] ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN4_PORT, IN4_PIN, HALF_STEP_SEQ[phase][3] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


/* ==============================
 *          초기화
 * ============================== */
void Stepper_Init(void)
{
  s_stepIndex = 0;
  s_prevTick = HAL_GetTick();
  s_busy = false;
  s_dir = DIR_UP;

  /* 초기 phase 출력
   * - 여기서는 phase 0을 출력하여 코일을 "일부 ON" 상태로 둘 수 있음(홀드)
   * - 완전 OFF를 원하면 아래 대신 모든 핀 RESET 처리 필요
   */
  Stepper_WritePhase(0);
}




/* ==============================
 *       연속 회전 시작
 * ============================== */
void Stepper_StartContinuous(uint8_t dir)
{

	/* dir 값은 DIR_UP / DIR_DOWN 사용 권장 */
	s_dir = dir;
	s_busy = true;
}


/* ==============================
 *           정지
 * ============================== */
void Stepper_Stop(void)
{
  s_busy = false;


  /* 주의: phase 0 출력은 "완전 OFF"가 아니라 0번 상으로 고정됨
   * - 홀드(토크 유지) 의도면 OK
   * - 코일 OFF(발열 감소/전력 절감) 의도면 아래 대신 모두 RESET 해야 함
   */


  Stepper_WritePhase(0);
  /* (완전 OFF가 필요하면 예시)
   * HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, GPIO_PIN_RESET);
   * HAL_GPIO_WritePin(IN2_PORT, IN2_PIN, GPIO_PIN_RESET);
   * HAL_GPIO_WritePin(IN3_PORT, IN3_PIN, GPIO_PIN_RESET);
   * HAL_GPIO_WritePin(IN4_PORT, IN4_PIN, GPIO_PIN_RESET);
   */
}


/* ==============================
 *      동작 상태 확인
 * ============================== */
bool Stepper_IsBusy(void) { return s_busy; }



/* ==============================
 *     논블로킹 스텝 진행 Task
 * ============================== */
void Stepper_Task(void)
{
	/* 회전 시작 상태가 아니면 아무 것도 하지 않음 */
	if (!s_busy) return;


	/* STEP_PERIOD_MS마다 1스텝 진행 */
	uint32_t now = HAL_GetTick();
	if (now - s_prevTick < STEP_PERIOD_MS) return;
	s_prevTick = now;

	/* 방향은 여기에서만 처리한다.
	 * - 엘리베이터 로직에서 phase 순서를 뒤집거나 인덱스를 조작하지 말 것
	 */
	if (s_dir == DIR_UP)
		s_stepIndex = (s_stepIndex + 1) & 0x07;
	else
		s_stepIndex = (s_stepIndex + 7) & 0x07;   // -1 mod 8

	Stepper_WritePhase(s_stepIndex);
}


