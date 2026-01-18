# 🏎️ Accelerator Control System
SeSAC Mobility Embedded Training Program의 Brake-by-Wire(BBW) 프로젝트 중 가속 제어 및 시스템 상태 모니터링 로직을 담당하는 저장소

## 📌 주요 기능
실시간 시동 제어: STM32 보드로부터 시리얼 데이터를 수신하여 릴레이를 통해 전원을 제어하고 시스템의 Engine ON/OFF 상태를 관리
정밀 모터 가속: 가변저항 센서 값을 읽어 모터 쉴드의 PWM 속도를 제어하며, 급제동 시 즉각적인 브레이크 모드를 활성화
RPM 및 상태 모니터링: 2상 엔코더와 인터럽트를 사용하여 실시간 RPM을 계산하고, I2C LCD에 현재 엔진 상태와 속도를 출력
서보 트리거 신호: 특정 브레이크 임계치 도달 시(BRAKE OVER 4000) 외부 STM32 장치로 제어 신호를 전송

## 📂 폴더 구조
* `sketch_jan14_accel/`: 가속 제어 메인 아두이노 소스 코드 (`.ino`)
* `libraries/`: 프로젝트 구동에 필요한 외부 라이브러리 (`LiquidCrystal_I2C`)

## 🛠 하드웨어 구성 (Pin Map)
| 기능 | 핀 번호 | 비고 |
|:---:|:---:|:---|
| **가변저항 (Pot)** | A2 | 가속 입력 |
| **모터 제어 (PWM)** | D3 | 속도 제어 |
| **모터 방향 (DIR)** | D12 | 방향 제어 |
| **브레이크 (Brake)** | D9 | 정지 및 유지 |
| **엔코더 (A상)** | D2 | 인터럽트 |
| **엔코더 (B상)** | D5 | 방향 판별용 |
| **UART RX** | D7 | 데이터 수신 (SoftwareSerial) |
| **LCD I2C** | SDA, SCL | 0x27 주소 사용 |
| **릴레이 제어** | D4 | STM32-B 전원 공급 제어 |
| **서보 신호** | D6 | STM32-B에 신호 전송 |
