/*
 * elevator.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */



/* elevator.c */
#include "elevator.h"
#include "button.h"
#include "stepper.h"
#include "servo.h"
#include "led.h"
#include "photo.h"
#include "logger.h"

#define DOOR_WAIT_MS_DEFAULT  6000
#define MOVE_TIMEOUT_MS       20000   // 안전 타임아웃(센서/기구 문제 대비)

static ELEVATOR_STATE s_state;
static uint8_t s_curFloor;     // 마지막 확정층(1~3)
static uint8_t s_targetFloor;  // 목표층(1~3)
static uint32_t s_doorTick;
static uint32_t s_moveStartTick;

static bool car_call[4];
static bool hall_up[4];
static bool hall_down[4];

static void ClearAllRequests(void)
{
  for (int f=1; f<=3; f++)
  {
    car_call[f]=false;
    hall_up[f]=false;
    hall_down[f]=false;
  }
}

/* ✅ 현재 층에서 정지해야 하는지 규칙 */
static bool ShouldStopHere(uint8_t floor, ELEVATOR_STATE movingState)
{
  if (car_call[floor]) return true;

  if (movingState == ELEVATOR_MOVING_UP)   return hall_up[floor];
  if (movingState == ELEVATOR_MOVING_DOWN) return hall_down[floor];

  return (hall_up[floor] || hall_down[floor]);
}

static void ConsumeStopRequests(uint8_t floor)
{
  car_call[floor]=false;
  hall_up[floor]=false;
  hall_down[floor]=false;
}

/* ✅ 다음 목적지 선택 (방향락 + 반대방향은 보류) */
static bool PickNextTarget(ELEVATOR_STATE moveDir, uint8_t *outTarget)
{
  if (moveDir == ELEVATOR_MOVING_UP)
  {
    for (int f=s_curFloor+1; f<=3; f++)
      if (car_call[f] || hall_up[f] || hall_down[f]) { *outTarget=(uint8_t)f; return true; }
    for (int f=s_curFloor-1; f>=1; f--)
      if (car_call[f] || hall_up[f] || hall_down[f]) { *outTarget=(uint8_t)f; return true; }
    return false;
  }

  if (moveDir == ELEVATOR_MOVING_DOWN)
  {
    for (int f=s_curFloor-1; f>=1; f--)
      if (car_call[f] || hall_up[f] || hall_down[f]) { *outTarget=(uint8_t)f; return true; }
    for (int f=s_curFloor+1; f<=3; f++)
      if (car_call[f] || hall_up[f] || hall_down[f]) { *outTarget=(uint8_t)f; return true; }
    return false;
  }

  /* IDLE: 가까운 요청 */
  for (int dist=0; dist<=2; dist++)
  {
    int up = (int)s_curFloor + dist;
    int dn = (int)s_curFloor - dist;

    if (up>=1 && up<=3 && (car_call[up] || hall_up[up] || hall_down[up])) { *outTarget=(uint8_t)up; return true; }
    if (dn>=1 && dn<=3 && (car_call[dn] || hall_up[dn] || hall_down[dn])) { *outTarget=(uint8_t)dn; return true; }
  }
  return false;
}

static void StartMoveTo(uint8_t target)
{
  if (target == s_curFloor) return;

  s_targetFloor = target;
  s_moveStartTick = HAL_GetTick();

  uint8_t dir = (target > s_curFloor) ? DIR_UP : DIR_DOWN;
  Stepper_StartContinuous(dir);

  s_state = (dir == DIR_UP) ? ELEVATOR_MOVING_UP : ELEVATOR_MOVING_DOWN;

  Log_Printf("MOVE START: cur=%u target=%u dir=%s\r\n",
             s_curFloor, s_targetFloor, (dir==DIR_UP)?"UP":"DOWN");
}

/* ✅ 포토로 현재층 갱신: 확정층(PF_Fx)일 때만 */
static void UpdateFloorFromPhoto(void)
{
  photo_fsm_t f = Photo_GetFSM();
  if (f == PF_F1) s_curFloor = 1;
  else if (f == PF_F2) s_curFloor = 2;
  else if (f == PF_F3) s_curFloor = 3;
}

/* ✅ 목표층 도착 여부 */
static bool ReachedTarget(void)
{
  photo_fsm_t f = Photo_GetFSM();
  if (s_targetFloor == 1 && f == PF_F1) return true;
  if (s_targetFloor == 2 && f == PF_F2) return true;
  if (s_targetFloor == 3 && f == PF_F3) return true;
  return false;
}

void Elevator_Init(void)
{
  s_state = ELEVATOR_IDLE;
  s_curFloor = 1;      // 초기값(포토 PF_F1 들어오면 자동 보정)
  s_targetFloor = 1;
  s_doorTick = 0;
  s_moveStartTick = 0;
  ClearAllRequests();
}

void Elevator_RequestCar(uint8_t floor)
{
  if (floor < 1 || floor > 3) return;
  car_call[floor] = true;
  Log_Printf("CAR CALL: %u\r\n", floor);
}

void Elevator_InputTask(void)
{
  if (Button_GetPressed(BTN_EMG))
  {
    s_state = ELEVATOR_EMG;
    Stepper_Stop();
    ledOff();
    Log_Printf("EMG STOP\r\n");
    return;
  }

  if (Button_GetPressed(BTN_OPEN))
  {
    if (s_state != ELEVATOR_EMG) s_state = ELEVATOR_DOOR_OPENING;
  }
  if (Button_GetPressed(BTN_CLOSE))
  {
    if (s_state != ELEVATOR_EMG) s_state = ELEVATOR_DOOR_CLOSING;
  }

  /* 내부 */
  if (Button_GetPressed(BTN_1F)) Elevator_RequestCar(1);
  if (Button_GetPressed(BTN_2F)) Elevator_RequestCar(2);
  if (Button_GetPressed(BTN_3F)) Elevator_RequestCar(3);

  /* 외부 */
  if (Button_GetPressed(BTN_1F_UP)) { hall_up[1]=true; Log_Printf("HALL UP 1\r\n"); }
  if (Button_GetPressed(BTN_2F_UP)) { hall_up[2]=true; Log_Printf("HALL UP 2\r\n"); }
  if (Button_GetPressed(BTN_2F_DW)) { hall_down[2]=true; Log_Printf("HALL DN 2\r\n"); }
  if (Button_GetPressed(BTN_3F_DW)) { hall_down[3]=true; Log_Printf("HALL DN 3\r\n"); }
}

void Elevator_Task(void)
{
  UpdateFloorFromPhoto();

  switch (s_state)
  {
    case ELEVATOR_EMG:
      ledOff();
      Stepper_Stop();
      return;

    case ELEVATOR_IDLE:
    {
      ledOff();

      if (ShouldStopHere(s_curFloor, ELEVATOR_IDLE))
      {
        ConsumeStopRequests(s_curFloor);
        s_state = ELEVATOR_DOOR_OPENING;
        Log_Printf("STOP@%u -> DOOR OPEN\r\n", s_curFloor);
        break;
      }

      uint8_t next;
      if (PickNextTarget(ELEVATOR_IDLE, &next))
      {
        if (next == s_curFloor) s_state = ELEVATOR_DOOR_OPENING;
        else StartMoveTo(next);
      }
      break;
    }

    case ELEVATOR_MOVING_UP:
    {
      ledUp();

      if (HAL_GetTick() - s_moveStartTick > MOVE_TIMEOUT_MS)
      {
        Stepper_Stop();
        ledOff();
        s_state = ELEVATOR_IDLE;
        Log_Printf("MOVE TIMEOUT -> IDLE\r\n");
        break;
      }

      if (ReachedTarget())
      {
        Stepper_Stop();
        Log_Printf("ARRIVE %u\r\n", s_curFloor);

        if (ShouldStopHere(s_curFloor, ELEVATOR_MOVING_UP))
        {
          ConsumeStopRequests(s_curFloor);
          s_state = ELEVATOR_DOOR_OPENING;
        }
        else
        {
          uint8_t next;
          if (PickNextTarget(ELEVATOR_MOVING_UP, &next)) StartMoveTo(next);
          else s_state = ELEVATOR_IDLE;
        }
      }
      break;
    }

    case ELEVATOR_MOVING_DOWN:
    {
      ledDown();

      if (HAL_GetTick() - s_moveStartTick > MOVE_TIMEOUT_MS)
      {
        Stepper_Stop();
        ledOff();
        s_state = ELEVATOR_IDLE;
        Log_Printf("MOVE TIMEOUT -> IDLE\r\n");
        break;
      }

      if (ReachedTarget())
      {
        Stepper_Stop();
        Log_Printf("ARRIVE %u\r\n", s_curFloor);

        if (ShouldStopHere(s_curFloor, ELEVATOR_MOVING_DOWN))
        {
          ConsumeStopRequests(s_curFloor);
          s_state = ELEVATOR_DOOR_OPENING;
        }
        else
        {
          uint8_t next;
          if (PickNextTarget(ELEVATOR_MOVING_DOWN, &next)) StartMoveTo(next);
          else s_state = ELEVATOR_IDLE;
        }
      }
      break;
    }

    case ELEVATOR_DOOR_OPENING:
      ledOff();
      Servo_Open();
      if (Servo_IsOpened())
      {
        s_doorTick = HAL_GetTick();
        s_state = ELEVATOR_DOOR_WAIT;
        Log_Printf("DOOR OPEN\r\n");
      }
      break;

    case ELEVATOR_DOOR_WAIT:
      ledOff();
      if (HAL_GetTick() - s_doorTick >= DOOR_WAIT_MS_DEFAULT)
      {
        s_state = ELEVATOR_DOOR_CLOSING;
      }
      break;

    case ELEVATOR_DOOR_CLOSING:
      ledOff();
      Servo_Close();
      if (Servo_IsClosed())
      {
        Log_Printf("DOOR CLOSE\r\n");
        uint8_t next;
        if (PickNextTarget(ELEVATOR_IDLE, &next))
        {
          if (next == s_curFloor) s_state = ELEVATOR_DOOR_OPENING;
          else StartMoveTo(next);
        }
        else
        {
          s_state = ELEVATOR_IDLE;
        }
      }
      break;
  }
}

uint8_t Elevator_GetCurrentFloor(void) { return s_curFloor; }
ELEVATOR_STATE Elevator_GetState(void) { return s_state; }


void Elevator_ResumeFromEMG(void)
{
  if (s_state == ELEVATOR_EMG)
  {
    s_state = ELEVATOR_IDLE;
  }
}


void Elevator_GetQueueString(char *out, uint32_t out_sz)
{
  if (!out || out_sz == 0) return;
  out[0] = 0;

  // 예: [C2 C3 HU1 HD2 ...] 형태로 출력
  char buf[64] = {0};
  uint32_t n = 0;

  n += (uint32_t)snprintf(buf+n, sizeof(buf)-n, "[");

  for (int f=1; f<=3; f++)
    if (car_call[f]) n += (uint32_t)snprintf(buf+n, sizeof(buf)-n, " C%d", f);
  for (int f=1; f<=3; f++)
    if (hall_up[f])  n += (uint32_t)snprintf(buf+n, sizeof(buf)-n, " HU%d", f);
  for (int f=1; f<=3; f++)
    if (hall_down[f])n += (uint32_t)snprintf(buf+n, sizeof(buf)-n, " HD%d", f);

  n += (uint32_t)snprintf(buf+n, sizeof(buf)-n, " ]");

  strncpy(out, buf, out_sz-1);
  out[out_sz-1] = 0;
}
