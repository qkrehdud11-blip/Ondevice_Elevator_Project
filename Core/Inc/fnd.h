/*
 * fnd.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_FND_H_
#define INC_FND_H_






#include "stm32f4xx_hal.h"


// ---------------------------------------------------------
// 1. 핀 정의 (Pin Definitions)
// ---------------------------------------------------------
#define HC595_PORT      GPIOC
#define HC595_SER_PIN   GPIO_PIN_10
#define HC595_SRCLK_PIN GPIO_PIN_11
#define HC595_RCLK_PIN  GPIO_PIN_12

#define DIG1_GPIO_PORT  GPIOA
#define DIG1_GPIO_PIN   GPIO_PIN_0
#define DIG2_GPIO_PORT  GPIOA
#define DIG2_GPIO_PIN   GPIO_PIN_1
#define DIG3_GPIO_PORT  GPIOD
#define DIG3_GPIO_PIN   GPIO_PIN_2
#define DIG4_GPIO_PORT  GPIOA
#define DIG4_GPIO_PIN   GPIO_PIN_15

// ---------------------------------------------------------
// 2. 비트 매핑 및 패턴 정의
// ---------------------------------------------------------
#define BIT_POS_B  0
#define BIT_POS_D  1
#define BIT_POS_E  2
#define BIT_POS_G  3
#define BIT_POS_C  4
#define BIT_POS_F  5
#define BIT_POS_A  6
#define BIT_POS_DP 7

#define SEG_A   (1 << BIT_POS_A)
#define SEG_B   (1 << BIT_POS_B)
#define SEG_C   (1 << BIT_POS_C)
#define SEG_D   (1 << BIT_POS_D)
#define SEG_E   (1 << BIT_POS_E)
#define SEG_F   (1 << BIT_POS_F)
#define SEG_G   (1 << BIT_POS_G)
#define SEG_DP  (1 << BIT_POS_DP)

// 특수 문자 패턴
#define PAT_F       (SEG_A | SEG_E | SEG_F | SEG_G)
#define PAT_CLOSE_1 (SEG_A | SEG_B | SEG_C | SEG_D)
#define PAT_CLOSE_2 (SEG_A | SEG_F | SEG_E | SEG_D)
#define PAT_OPEN_1  (SEG_F | SEG_E)
#define PAT_OPEN_2  (SEG_B | SEG_C)
#define PAT_BLANK   0x00

// 숫자 패턴 (0~9)
static const uint8_t NUM_PATTERNS[] = {
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,       // 0
    SEG_B|SEG_C,                               // 1
    SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,             // 2
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,             // 3
    SEG_B|SEG_C|SEG_F|SEG_G,                   // 4
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,             // 5
    SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,       // 6
    SEG_A|SEG_B|SEG_C,                         // 7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G, // 8
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G        // 9
};


// --- 공개 함수 프로토타입 ---

/**
 * @brief FND 상태(층수, 문열림)를 설정한다. (논 블로킹)
 * @param floor: 표시할 층수 (0~9)
 * @param isDoorOpen: 0=닫힘(][), 1=열림(| |)
 */
void setFndState(uint8_t floor, uint8_t isDoorOpen);

/**
 * @brief FND 화면을 갱신한다. (블로킹, 약 10ms 소요)
 * @note  잔상 효과를 위해 while(1) 루프에서 지속적으로 호출되어야 한다.
 */
void displayScan(void);






#endif /* INC_FND_H_ */
