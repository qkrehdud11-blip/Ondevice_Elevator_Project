/*
 * button.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */




/* button.c */
#include "button.h"

/* 너가 올린 핀 매핑 그대로 반영 */
static BUTTON_CONTROL button[BUTTON_COUNT] =
{
    /* 외부 */
    {GPIOB, GPIO_PIN_12, GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 1F UP
    {GPIOB, GPIO_PIN_2,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 2F UP
    {GPIOB, GPIO_PIN_1,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 2F DW
    {GPIOB, GPIO_PIN_15, GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 3F DW

    /* 내부 */
    {GPIOB, GPIO_PIN_14, GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // CLOSE
    {GPIOB, GPIO_PIN_13, GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // OPEN
    {GPIOC, GPIO_PIN_4,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 1F
    {GPIOC, GPIO_PIN_9,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 2F
    {GPIOA, GPIO_PIN_5,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // 3F
    {GPIOA, GPIO_PIN_6,  GPIO_PIN_RESET, 0, GPIO_PIN_SET, GPIO_PIN_SET}, // EMG
};


void ButtonInit(void)
{
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
        GPIO_PinState raw = HAL_GPIO_ReadPin(button[i].port, button[i].pin);
        button[i].rawPrev = raw;
        button[i].lastChangeTick = now;

        button[i].stable = raw;
        button[i].stablePrev = raw;
    }
}

bool Button_GetPressed(uint8_t num)
{
    if (num >= BUTTON_COUNT) return false;

    uint32_t now = HAL_GetTick();
    GPIO_PinState raw = HAL_GPIO_ReadPin(button[num].port, button[num].pin);

    // RAW 변화 감지 → 타이머 리셋
    if (raw != button[num].rawPrev)
    {
        button[num].rawPrev = raw;
        button[num].lastChangeTick = now;
    }

    // DEBOUNCE_TIME 동안 RAW가 유지되면 stable로 채택
    if ((now - button[num].lastChangeTick) >= DEBOUNCE_TIME)
    {
        if (button[num].stable != raw)
        {
            button[num].stablePrev = button[num].stable;
            button[num].stable = raw;
        }
    }

    // 안정 레벨 기준: 안눌림 -> 눌림 (onState) 1회 이벤트
    if (button[num].stablePrev != button[num].stable)
    {
        // 변화가 있었던 “그 순간”만 처리되게 stablePrev 갱신
        button[num].stablePrev = button[num].stable;

        if (button[num].stable == button[num].onState)
            return true;
    }

    return false;
}









