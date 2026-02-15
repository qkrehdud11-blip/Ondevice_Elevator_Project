/*
 * photo.c
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 *
 *
 *  -  포토센서 3개 입력을 읽어 층/이동구간을 판단한다.
 *  - 빔이 끊기면 LOW(RESET)인 센서 기준으로 작성됨
 *  - EXTI 발생 시 pending 플래그만 세우고, 실제 판독은 Task에서 디바운스 후 수행
 *  - EXTI 놓침 대비 폴링도 함께 수행
 */


#include "photo.h"

/* ==============================
 *         핀 매핑
 * ============================== */
#define PHOTO1_PORT GPIOA
#define PHOTO1_PIN  GPIO_PIN_12

#define PHOTO2_PORT GPIOB
#define PHOTO2_PIN  GPIO_PIN_7

#define PHOTO3_PORT GPIOB
#define PHOTO3_PIN  GPIO_PIN_10

/* 일반적인 포토센서 모듈: 빔 끊기면 LOW */
#define PHOTO_ACTIVE_STATE GPIO_PIN_RESET


/* 해당 핀이 "감지" 상태인지(1/0)로 변환 */
static inline uint8_t Detected(GPIO_TypeDef *port, uint16_t pin)
{
  return (HAL_GPIO_ReadPin(port, pin) == PHOTO_ACTIVE_STATE) ? 1 : 0;
}


/* EXTI 디바운스 시간 */
#define PHOTO_DEBOUNCE_MS  30


/* ==============================
 *        내부 상태 변수
 * ============================== */
static volatile uint8_t  s_irq_pending = 0;   // EXTI 발생 여부(플래그)
static volatile uint32_t s_irq_tick = 0;      // EXTI 발생 시각

static uint8_t  s_p1=0, s_p2=0, s_p3=0;       // 마지막으로 확정된 RAW 값
static uint8_t  s_valid = 0;                  // 초기값 읽었는지 표시(사실상 항상 1로 유지)
static uint32_t s_boot_tick = 0;              // 부팅 직후 튐 방지용


/* 현재 RAW를 즉시 읽는 함수 */
static void ReadNow(uint8_t *o1, uint8_t *o2, uint8_t *o3)
{
  *o1 = Detected(PHOTO1_PORT, PHOTO1_PIN);
  *o2 = Detected(PHOTO2_PORT, PHOTO2_PIN);
  *o3 = Detected(PHOTO3_PORT, PHOTO3_PIN);
}


/* RAW 조합을 FSM 상태로 변환 */
static photo_fsm_t DecodeFSM(uint8_t p1, uint8_t p2, uint8_t p3)
{
  uint8_t sum = (uint8_t)(p1 + p2 + p3);

  /* 아무 것도 감지 안됨 -> 이동중으로 표시 */
  if (sum == 0) return PF_UNKNOWN;

  /* 정확히 하나만 감지되면 해당 층 */
  if (sum == 1)
  {
    if (p1) return PF_F1;
    if (p2) return PF_F2;
    if (p3) return PF_F3;
  }

  /* 두 개가 동시에 감지되면 층 사이 구간 */
  if (sum == 2)
  {
    if (p1 && p2 && !p3) return PF_MOVE_1_2;
    if (!p1 && p2 && p3) return PF_MOVE_2_3;
    return PF_ERROR;   // p1&p3 같이 비정상
  }

  /* 3개 모두 감지 등은 비정상 */
  return PF_ERROR;
}

void Photo_Init(void)
{
  s_irq_pending = 0;
  s_irq_tick = 0;
  s_boot_tick = HAL_GetTick();

  /* 부팅 직후 현재 상태를 1회 읽어서 기준값으로 사용 */
  ReadNow(&s_p1, &s_p2, &s_p3);
  s_valid = 1;
}

/* EXTI 콜백에서 호출: 실제 읽기는 Task에서 디바운스 후 처리 */
void Photo_OnExti(uint16_t GPIO_Pin)
{
  /* 부팅 직후 센서 튐/불안정 방지 */
  if (HAL_GetTick() - s_boot_tick < 200) return;

  /* 이 구현은 핀 번호만 비교(포트까지는 확인 안함) */
  if (GPIO_Pin == PHOTO1_PIN || GPIO_Pin == PHOTO2_PIN || GPIO_Pin == PHOTO3_PIN)
  {
    s_irq_pending = 1;
    s_irq_tick = HAL_GetTick();
  }
}

/* 변경이 있으면 1 반환 */
uint8_t Photo_Task(void)
{
  uint32_t now = HAL_GetTick();
  uint8_t changed = 0;

  /* 1) EXTI 기반 갱신 */
  if (s_irq_pending)
  {
    /* 디바운스 시간 대기 */
    if (now - s_irq_tick < PHOTO_DEBOUNCE_MS) return 0;
    s_irq_pending = 0;

    uint8_t n1,n2,n3;
    ReadNow(&n1,&n2,&n3);

    if (n1!=s_p1 || n2!=s_p2 || n3!=s_p3)
    {
      s_p1=n1; s_p2=n2; s_p3=n3;
      changed = 1;
    }
  }

  /* 2) 백업 폴링(EXTI miss 대비) */
  static uint32_t pollTick = 0;
  if (now - pollTick >= 50)
  {
    pollTick = now;

    uint8_t n1,n2,n3;
    ReadNow(&n1,&n2,&n3);

    if (n1!=s_p1 || n2!=s_p2 || n3!=s_p3)
    {
      s_p1=n1; s_p2=n2; s_p3=n3;
      changed = 1;
    }
  }

  return changed;
}

void Photo_GetRaw(uint8_t *p1, uint8_t *p2, uint8_t *p3)
{
  if (p1) *p1 = s_p1;
  if (p2) *p2 = s_p2;
  if (p3) *p3 = s_p3;
}

photo_fsm_t Photo_GetFSM(void)
{
  if (!s_valid) return PF_UNKNOWN;
  return DecodeFSM(s_p1, s_p2, s_p3);
}

const char* Photo_FSM_ToString(photo_fsm_t f)
{
  switch (f)
  {
    case PF_F1:       return "FLOOR=1";
    case PF_F2:       return "FLOOR=2";
    case PF_F3:       return "FLOOR=3";
    case PF_MOVE_1_2: return "MOVE 1~2";
    case PF_MOVE_2_3: return "MOVE 2~3";
    case PF_UNKNOWN:  return "MOVING";
    case PF_ERROR:    return "ERROR";
    default:          return "?";
  }
}
