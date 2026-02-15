/*
 * floor_fsm.h
 *
 *  Created on: Feb 9, 2026
 *      Author: parkdoyoung
 */

#ifndef INC_FLOOR_FSM_H_
#define INC_FLOOR_FSM_H_








#include <stdint.h>

/* 포토 기반 층 상태 */
typedef enum
{
  FS_UNKNOWN = 0,

  FS_F1,
  FS_F2,
  FS_F3,

  FS_MOVE_1_2,   // 1~2 사이 (P1=1,P2=1,P3=0)
  FS_MOVE_2_3,   // 2~3 사이 (P1=0,P2=1,P3=1)

  FS_ERROR        // 이상한 조합(예: P1=1,P3=1)
} floor_state_t;

floor_state_t FloorFSM_Decode(uint8_t p1, uint8_t p2, uint8_t p3);
const char* FloorFSM_ToString(floor_state_t s);







#endif /* INC_FLOOR_FSM_H_ */
