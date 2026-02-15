/*
 * led.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */



#include "led.h"
#include "delay.h"

// --- 정적 변수 (상태 저장) ---
static LedMode_t last_mode = MODE_NONE; // 마지막으로 실행된 모드 기억
static uint32_t last_tick = 0;          // 시간 기록
static uint8_t step_index = 0;          // 진행 단계 (0~7)
static uint8_t current_pattern = 0;     // 현재 LED 모양

// --- 하드웨어 제어 (Private) ---
static void dataOut(uint8_t data)
{
  for(int i = 7; i >= 0; i--)
  {
    if(data & (1 << i)) HAL_GPIO_WritePin(GPIO_PORT, SER_PIN, GPIO_PIN_SET);
    else                HAL_GPIO_WritePin(GPIO_PORT, SER_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIO_PORT, SRCLK_PIN, GPIO_PIN_SET);
    delay_us(5);
    HAL_GPIO_WritePin(GPIO_PORT, SRCLK_PIN, GPIO_PIN_RESET);
    delay_us(5);
  }
  HAL_GPIO_WritePin(GPIO_PORT, RCLK_PIN, GPIO_PIN_SET);
  delay_us(10);
  HAL_GPIO_WritePin(GPIO_PORT, RCLK_PIN, GPIO_PIN_RESET);
}

// --- 공통 초기화 로직 (모드가 바뀔 때 호출) ---
static void ResetAnimation(LedMode_t new_mode)
{
    last_mode = new_mode;
    step_index = 0;
    current_pattern = 0;
    last_tick = HAL_GetTick();
    dataOut(0x00); // 일단 끔
}

// --- 공개 함수 ---

void LED_Init(void)
{
    ResetAnimation(MODE_OFF);
}

void ledUp(void)
{
    // 1. 모드 전환 감지: 방금 전까지 Down이나 Off였다면 초기화
    if (last_mode != MODE_UP)
    {
        ResetAnimation(MODE_UP);
    }

    // 2. 시간 체크 (300ms) - 논 블로킹
    uint32_t now = HAL_GetTick();
    if ((now - last_tick) < 300) return;

    // 3. 로직 실행
    last_tick = now;

    current_pattern |= (1 << step_index); // 아래에서 위로 쌓기
    dataOut(current_pattern);
    step_index++;

    // 4. 끝까지 다 찼으면 다시 0부터 시작 (반복 애니메이션)
    if (step_index >= 8)
    {
        step_index = 0;
        current_pattern = 0;
    }
}

void ledDown(void)
{
    // 1. 모드 전환 감지
    if (last_mode != MODE_DOWN)
    {
        ResetAnimation(MODE_DOWN);
    }

    // 2. 시간 체크
    uint32_t now = HAL_GetTick();
    if ((now - last_tick) < 300) return;

    // 3. 로직 실행
    last_tick = now;

    current_pattern |= (0x80 >> step_index); // 위에서 아래로 쌓기
    dataOut(current_pattern);
    step_index++;

    // 4. 반복 초기화
    if (step_index >= 8)
    {
        step_index = 0;
        current_pattern = 0;
    }
}

void ledOff(void)
{
    // Off는 애니메이션이 아니라 즉시 꺼져야 함
    // 계속 호출되더라도 부하를 줄이기 위해 상태 체크
    if (last_mode != MODE_OFF)
    {
        dataOut(0x00); // 끄기
        last_mode = MODE_OFF;
    }
}
