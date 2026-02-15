/*
 * photo.h
 *
 *  Created on: Feb 8, 2026
 *      Author: parkdoyoung
 *
 *  - 포토센서(층 감지) 모듈
 *  - 3개의 포토센서 조합으로 1/2/3층 + 이동구간(1~2, 2~3)을 판별
 *  - EXTI 인터럽트로 변화 감지 + 디바운스 + 백업 폴링(미스 대비)
 */

#ifndef INC_PHOTO_H_
#define INC_PHOTO_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>



/* ==============================
 *        포토 상태(FSM)
 * ==============================
 * PF_UNKNOWN : RAW=000 등 "아무것도 감지 안됨" (이동중 표시로 사용)
 * PF_F1/F2/F3: 해당 층 센서 1개만 감지됨
 * PF_MOVE_*  : 두 센서가 동시에 감지되는 구간(층 사이)
 * PF_ERROR   : 비정상 조합(예: p1&p3 같이 말이 안되는 조합)
 */
typedef enum {
  PF_UNKNOWN = 0,
  PF_F1,
  PF_F2,
  PF_F3,
  PF_MOVE_1_2,
  PF_MOVE_2_3,
  PF_ERROR
} photo_fsm_t;

void Photo_Init(void);


/**
 * @brief EXTI 콜백에서 호출 (어떤 포토 핀에서 인터럽트가 났는지 전달)
 * @note  현재 구현은 GPIO_Pin 값으로만 판별(포트까지는 확인하지 않음)
 */
void Photo_OnExti(uint16_t GPIO_Pin);


/**
 * @brief  주기적으로 호출하는 Task
 * @retval 1: 값이 바뀜(상태 갱신됨), 0: 변화 없음
 */
uint8_t Photo_Task(void);

void Photo_GetRaw(uint8_t *p1, uint8_t *p2, uint8_t *p3);
photo_fsm_t Photo_GetFSM(void);
const char* Photo_FSM_ToString(photo_fsm_t f);

#endif /* INC_PHOTO_H_ */
