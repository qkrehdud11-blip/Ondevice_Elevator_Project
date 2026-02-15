/*
 * led.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_LED_H_
#define INC_LED_H_










#include "stm32f4xx_hal.h"

#define SER_PIN       GPIO_PIN_5
#define SRCLK_PIN     GPIO_PIN_6
#define RCLK_PIN      GPIO_PIN_8
#define GPIO_PORT     GPIOC

// --- 내부 동작 모드 정의 ---
typedef enum {
    MODE_NONE,
    MODE_UP,
    MODE_DOWN,
    MODE_OFF
} LedMode_t;


// 초기화
void LED_Init(void);

// 엘리베이터 상태에 따라 호출할 논 블로킹 함수들
void ledUp(void);   // 올라갈 때 계속 호출
void ledDown(void); // 내려갈 때 계속 호출
void ledOff(void);  // 멈췄을 때 호출












#endif /* INC_LED_H_ */
