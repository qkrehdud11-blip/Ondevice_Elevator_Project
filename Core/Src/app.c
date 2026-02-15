/*
 * app.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#include "app.h"
#include "button.h"
#include "elevator.h"
#include "servo.h"
#include "stepper.h"
#include "led.h"
#include "photo.h"

#include "logger.h"
#include "usart.h"
#include "fnd.h"
#include "resident_uart.h"


//static void Debug_PrintPhotoPeriodic(void)
//{
//  static uint32_t last = 0;
//  if (HAL_GetTick() - last < 200) return;
//  last = HAL_GetTick();
//
//  uint8_t p1,p2,p3;
//  Photo_GetRaw(&p1,&p2,&p3);
//
//  photo_fsm_t f = Photo_GetFSM();
//  Log_Printf("RAW=%u%u%u %s\r\n", p1,p2,p3, Photo_FSM_ToString(f));
//}

void App_Init(void)
{
  Log_Init(&huart2);
  Log_Printf("UART2 CMD READY\r\n");

  ButtonInit();
  LED_Init();
  Servo_Init();
  Stepper_Init();
  Photo_Init();
  Elevator_Init();

  Log_Init(&huart2);
  ResidentUART_Init(&huart2);
}

void App_Task(void)
{
  displayScan();

  /* 센서/상태 갱신 */
  if (Photo_Task())
  {
    // 변화 있었을 때만 찍고 싶으면 여기서 찍어도 됨
  }
//  Debug_PrintPhotoPeriodic();

  /* 입력/정책/상태머신 */
  Elevator_InputTask();
  Elevator_Task();

  /* 구동부 */
  Stepper_Task();
  Servo_Run();

  /* FND */
  uint8_t doorOpen = Servo_IsOpened() ? 1 : 0;
  setFndState(Elevator_GetCurrentFloor(), doorOpen);

  ResidentUART_Task();
}
