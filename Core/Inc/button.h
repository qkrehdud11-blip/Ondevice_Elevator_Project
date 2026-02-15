/*
 * button.h
 *
 *  Created on: Jan 27, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_





#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define BUTTON_COUNT    10
#define DEBOUNCE_TIME   60   // ms

typedef enum
{
    BTN_1F_UP,
    BTN_2F_UP,
    BTN_2F_DW,
    BTN_3F_DW,
    BTN_CLOSE,
    BTN_OPEN,
    BTN_1F,
    BTN_2F,
    BTN_3F,
    BTN_EMG
} BUTTON_table;

typedef struct
{
    GPIO_TypeDef   *port;
    uint16_t        pin;
    GPIO_PinState   onState;

    GPIO_PinState   rawPrev;        // ★ 직전 RAW
    uint32_t        lastChangeTick; // ★ RAW 바뀐 시점

    GPIO_PinState   stable;         // 안정화 레벨
    GPIO_PinState   stablePrev;     // 직전 안정화 레벨
} BUTTON_CONTROL;

void ButtonInit(void);
bool Button_GetPressed(uint8_t num);




#endif /* INC_BUTTON_H_ */
