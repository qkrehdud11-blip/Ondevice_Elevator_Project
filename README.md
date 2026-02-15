# 🏢 CallTalkEli  
### STM32 기반 UART 연동 스마트 엘리베이터 시스템

---

## 📌 Project Overview

**CallTalkEli**는 STM32 기반 3층 미니 엘리베이터 임베디드 프로젝트입니다.

사용자는 버튼뿐만 아니라 **UART(Moserial)**를 통해 직접 엘리베이터를 호출할 수 있으며,  
현재 층, 이동 상태, 도어 상태를 실시간으로 확인할 수 있습니다.

또한 **Photo Interrupter 센서 기반 층 위치 인식 시스템**을 적용하여  
실제 층 도달 여부를 하드웨어 인터럽트로 감지합니다.

> Call → Talk → Elevator  
> 호출하고, 상태를 확인하는 인터랙티브 엘리베이터 시스템

---

## 🎯 Key Features

- UART 기반 층 호출 기능 (`call 1`, `call 2`, `call 3`)
- 실시간 상태 출력 (층 / 이동중 / 도어 상태)
- Photo Interrupter 기반 층 인식 (EXTI 인터럽트)
- 상태머신 기반 구조 설계
- Stepper Motor + Servo Motor 통합 제어
- Moserial 연동 모니터링

---

## 🔄 System Flow

1. 버튼 또는 UART로 층 호출  
2. 목표 층 설정  
3. 스텝모터 구동 (상행 / 하행)  
4. 이동 중 상태 UART 출력  
5. Photo Interrupter 센서로 층 감지  
6. 도착 시 자동 도어 오픈  
7. 일정 시간 대기 후 자동 도어 클로즈  

---

## 🖥️ UART Command (Moserial)

### ▶ Floor Call

```text
call 1
call 2
call 3
```

입력 시 해당 층으로 이동합니다.

---

### ▶ Example Output

```text
BOOT OK
FLOOR=1
CALL RECEIVED: 3
MOVING UP
FLOOR=2
MOVING 2~3
FLOOR=3
DOOR OPEN
```

---

## 📍 Floor Detection System

층 위치는 **Photo Interrupter 센서**를 통해 감지합니다.

- 각 층에 센서 배치
- EXTI 인터럽트 방식으로 신호 감지
- 이동 중/도착 상태 판단

예시 출력:

```text
FLOOR=1
MOVE 1~2
FLOOR=2
```

이를 통해 단순 스텝 카운트가 아닌  
**실제 센서 기반 위치 인식 구조**를 구현했습니다.

---

## 🔘 Button Configuration

### External Call Buttons
- 1F ▲  
- 2F ▲ / ▼  
- 3F ▼  

### Internal Control
- 1 / 2 / 3 Floor Button  
- OPEN  
- CLOSE  
- EMG (Emergency Stop – 즉시 모터 정지)

---

## ⚙️ Hardware Configuration

| Component | Description |
|-----------|------------|
| MCU | STM32 |
| Lift Motor | Stepper Motor |
| Door Motor | Servo Motor |
| Floor Sensor | Photo Interrupter |
| Interface | Button Input |
| Debug | UART (DMA) |
| Monitor Tool | Moserial |

---

## 🧠 Elevator State Machine

```c
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
```

모든 동작은 상태 전이에 따라 제어됩니다.

---

## 📁 Project Structure

### 📂 Core/Src

- `main.c` – Main loop & system entry point  
- `app.c` – Overall system control logic  
- `elevator.c` – Elevator state machine implementation  
- `stepper.c` – Lift motor (stepper) control  
- `servo.c` – Door open/close control  
- `button.c` – Button input handling & debouncing  
- `photo.c` – Photo interrupter FSM (floor detection)  
- `resident_uart.c` – UART command processing  
- `logger.c` – Debug logging output  


---

## 💬 User Experience

- 버튼으로 호출 가능  
- UART로 직접 호출 가능  
- 현재 위치 실시간 확인 가능  
- 이동 중 상태 확인 가능  
- 도어 상태 확인 가능  

엘리베이터를 단순 제어 장치가 아닌  
**상태를 보여주는 시스템**으로 구현했습니다.

---

## 👤 Author

**PARK DOYOUNG**  
Embedded System Project  
CallTalkEli – Smart UART Elevator
