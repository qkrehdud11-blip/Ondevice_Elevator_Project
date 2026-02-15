/*
 * logger.c
 *
 *  - Log_Printf()는 printf처럼 사용 가능
 *  - 내부에서 vsnprintf로 문자열 생성 후
 *    HAL_UART_Transmit()로 전송
 */

#include "logger.h"
#include <stdio.h>
#include <string.h>

/* 등록된 UART 핸들 */
static UART_HandleTypeDef *s_huart = 0;

/* ==============================
 *        초기화
 * ============================== */
void Log_Init(UART_HandleTypeDef *huart)
{
  s_huart = huart;
}

/* ==============================
 *        printf 출력 함수
 * ============================== */
void Log_Printf(const char *fmt, ...)
{
  if (!s_huart) return;   // UART 등록 안 됐으면 무시

  char buf[160];          // 출력 버퍼

  va_list ap;
  va_start(ap, fmt);

  /* printf 포맷 문자열을 buf에 작성 */
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);

  va_end(ap);

  if (n <= 0) return;     // 변환 실패

  /*
   * vsnprintf 특징:
   *  - 버퍼보다 긴 문자열이면 잘리지만
   *  - n은 "원래 필요 길이"를 반환할 수 있음
   * 따라서 실제 전송 길이는 버퍼 크기 내로 제한
   */
  if (n >= (int)sizeof(buf))
  {
    n = (int)sizeof(buf) - 1;
    buf[n] = '\0';
  }

  /* 블로킹 전송 (타임아웃 50ms) */
  HAL_UART_Transmit(s_huart, (uint8_t*)buf, (uint16_t)n, 50);
}
