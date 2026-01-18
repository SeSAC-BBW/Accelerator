# 🏎️ Accelerator Control System (SeSAC-BBW Team)

**SeSAC Mobility Embedded Training Program**의 **Brake-by-Wire(BBW)** 프로젝트 중 가속 제어 및 시스템 상태 모니터링 로직을 담당하는 Repository

## 📌 주요 기능 (Core Features)

* [cite_start]**실시간 시동 제어** STM32 보드로부터 시리얼 데이터를 수신하여 릴레이를 통해 전원을 제어하고 시스템의 Engine ON/OFF 상태를 관리 [cite: 16, 18, 21]

* [cite_start]**정밀 모터 가속** 가변저항 센서 값을 읽어 모터 쉴드의 PWM 속도를 제어하며, 엔진 상태에 따라 즉각적인 브레이크 모드를 활성화 [cite: 25, 27, 28]

* [cite_start]**RPM 및 상태 모니터링** 2상 엔코더와 인터럽트를 사용하여 실시간 RPM을 계산하고, I2C LCD에 현재 엔진 상태와 속도를 출력 [cite: 7, 32, 33]

* [cite_start]**서보 트리거 신호** 특정 브레이크 임계치 도달 시(BRAKE OVER 4000) 외부 STM32 장치로 제어 신호를 전송 [cite: 22, 23]

## 🛠 하드웨어 구성 (Hardware Pin Map)

| 구분 | 기능 | 핀 번호 | I/O | 상세 설명 |
| :---: | :--- | :---: | :---: | :--- |
| **Control** | 릴레이 제어 (Relay) | **D4** | Output | [cite_start]STM32-B 전원 공급 제어 [cite: 6] |
| **Control** | 서보 신호 (Trigger) | **D6** | Output | [cite_start]**STM32-B**의 PB13으로 비상 제동 신호 전송 [cite: 6] |
| **Motor** | 속도 제어 (PWM) | **D3** | Output | [cite_start]모터 출력 강도 조절 [cite: 3] |
| **Motor** | 방향 제어 (DIR) | **D12** | Output | [cite_start]모터 회전 방향 설정 [cite: 2] |
| **Motor** | 브레이크 (Brake) | **D9** | Output | [cite_start]모터 잠금 및 해제 제어 [cite: 4] |
| **Sensor** | 가변저항 (Pot) | **A2** | Input | [cite_start]가속 페달 입력값 수신 [cite: 4] |
| **Sensor** | 엔코더 A상 (Red) | **D2** | Input | [cite_start]**Interrupt** (RPM 측정) [cite: 5] |
| **Sensor** | 엔코더 B상 (Yellow) | **D5** | Input | [cite_start]회전 방향 판별 [cite: 5] |
| **Comm.** | UART RX | **D7** | Input | [cite_start]**STM32-A**의 PC10(TX)와 연결되어 수신 (SoftwareSerial) [cite: 1, 2] |
| **Comm.** | LCD I2C (SDA/SCL) | **SDA/SCL** | I2C | [cite_start]시스템 상태 실시간 표시 (Addr: 0x27) [cite: 1] |

## 📂 폴더 구조 (Folder Structure)

* `sketch_jan14_accel/`: 가속 제어 아두이노 메인 소스 코드 폴더
* `libraries/`: `LiquidCrystal_I2C` 등 필수 외부 라이브러리 폴더
