/*
 * fnd.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */



#include "fnd.h"
#include "delay.h"

// 화면 버퍼 (내부 전역 변수)
static uint8_t display_buffer[4] = {PAT_BLANK, PAT_BLANK, PAT_BLANK, PAT_BLANK};


// ---------------------------------------------------------
// 1. 내부 함수 (Private Functions)
// ---------------------------------------------------------

static void dataOut(uint8_t data)
{
  for(int i = 7; i >= 0; i--)
  {
    if(data & (1 << i)) HAL_GPIO_WritePin(HC595_PORT, HC595_SER_PIN, GPIO_PIN_SET);
    else                HAL_GPIO_WritePin(HC595_PORT, HC595_SER_PIN, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(HC595_PORT, HC595_SRCLK_PIN, GPIO_PIN_SET);
    delay_us(1);
    HAL_GPIO_WritePin(HC595_PORT, HC595_SRCLK_PIN, GPIO_PIN_RESET);
    delay_us(1);
  }

  HAL_GPIO_WritePin(HC595_PORT, HC595_RCLK_PIN, GPIO_PIN_SET);
  delay_us(1);
  HAL_GPIO_WritePin(HC595_PORT, HC595_RCLK_PIN, GPIO_PIN_RESET);
}

static void DIG_Select(uint8_t digit_pos) {
    switch (digit_pos) {
        case 0: HAL_GPIO_WritePin(DIG1_GPIO_PORT, DIG1_GPIO_PIN, GPIO_PIN_RESET); break;
        case 1: HAL_GPIO_WritePin(DIG2_GPIO_PORT, DIG2_GPIO_PIN, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(DIG3_GPIO_PORT, DIG3_GPIO_PIN, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(DIG4_GPIO_PORT, DIG4_GPIO_PIN, GPIO_PIN_RESET); break;
    }
}

static void DIG_Off(void) {
    HAL_GPIO_WritePin(DIG1_GPIO_PORT, DIG1_GPIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DIG2_GPIO_PORT, DIG2_GPIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DIG3_GPIO_PORT, DIG3_GPIO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DIG4_GPIO_PORT, DIG4_GPIO_PIN, GPIO_PIN_SET);
}


// ---------------------------------------------------------
// 2. 공개 함수 (Public Functions)
// ---------------------------------------------------------

void setFndState(uint8_t floor, uint8_t isDoorOpen)
{
    // 1. 문 상태 설정
    if (isDoorOpen) {
        display_buffer[0] = PAT_OPEN_1;
        display_buffer[1] = PAT_OPEN_2;
    } else {
        display_buffer[0] = PAT_CLOSE_1;
        display_buffer[1] = PAT_CLOSE_2;
    }

    // 2. 층수 표시
    if (floor <= 9) {
        display_buffer[2] = NUM_PATTERNS[floor];
    } else {
        display_buffer[2] = PAT_BLANK;
    }

    // 3. 단위 표시 'F'
    display_buffer[3] = PAT_F;
}

void displayScan(void) {
    for (uint8_t i = 0; i < 4; i++) {
        // 잔상 제거를 위해 끄기
        DIG_Off();

        // 데이터 전송
        dataOut(display_buffer[i]);

        // 자리 켜기
        DIG_Select(i);

        // 최대 밝기 유지 (2.5ms)
        delay_us(2500);

        // 끄는 시간 없음 (Duty 100%)
    }
}
