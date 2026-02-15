/*
 * elevator.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_


#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  ELEVATOR_IDLE,
  ELEVATOR_MOVING_UP,
  ELEVATOR_MOVING_DOWN,
  ELEVATOR_DOOR_OPENING,
  ELEVATOR_DOOR_WAIT,
  ELEVATOR_DOOR_CLOSING,
  ELEVATOR_EMG
} ELEVATOR_STATE;

void Elevator_Init(void);
void Elevator_InputTask(void);
void Elevator_Task(void);

void Elevator_RequestCar(uint8_t floor);   // UART/내부버튼 공용
uint8_t Elevator_GetCurrentFloor(void);
ELEVATOR_STATE Elevator_GetState(void);

void Elevator_ResumeFromEMG(void);
void Elevator_GetQueueString(char *out, uint32_t out_sz);


#endif /* INC_ELEVATOR_H_ */
