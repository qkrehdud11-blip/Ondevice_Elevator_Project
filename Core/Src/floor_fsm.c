/*
 * floor_fsm.c
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */


#include "floor_fsm.h"

floor_state_t FloorFSM_Decode(uint8_t p1, uint8_t p2, uint8_t p3)
{
  uint8_t sum = (uint8_t)(p1 + p2 + p3);

  if (sum == 0) return FS_UNKNOWN;

  // 하나만 1이면 확정층
  if (sum == 1)
  {
    if (p1) return FS_F1;
    if (p2) return FS_F2;
    if (p3) return FS_F3;
  }

  // 두 개가 1이면 "사이" 구간
  if (sum == 2)
  {
    if (p1 && p2 && !p3) return FS_MOVE_1_2; // 1~2
    if (!p1 && p2 && p3) return FS_MOVE_2_3; // 2~3
    return FS_ERROR; // (p1&p3) 같은 비정상
  }

  // 3개 다 1이면 비정상
  return FS_ERROR;
}

const char* FloorFSM_ToString(floor_state_t s)
{
  switch (s)
  {
    case FS_F1:       return "FLOOR=1";
    case FS_MOVE_1_2: return "MOVE 1~2";
    case FS_F2:       return "FLOOR=2";
    case FS_MOVE_2_3: return "MOVE 2~3";
    case FS_F3:       return "FLOOR=3";
    case FS_UNKNOWN:  return "MOVING";
    case FS_ERROR:    return "ERROR";
    default:          return "?";
  }
}
