#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C 주소 설정 & 16칸 2줄
SoftwareSerial mySerial(7, -1);     // 7을 RX로 설정 (STM32의 PC10과 연결)

// 모터 쉴드 R3 고정 핀
const int dir = 12;      // 회전 방향 제어
const int pwm = 3;       // 속도 제어 (PWM)
const int brake = 9;     // 브레이크

// 가변저항 센서 및 엔코더 핀
const int potPin = A2;            // 가변저항
const byte encoderPinRed = 2;     // 엔코더 빨간색 선
const byte encoderPinYellow = 5;  // 엔코더 노란색 선

const int servoSignalPin = 6; // 다른 하드웨어로 신호를 보낼 핀
const int relayPin = 4; // 릴레이 제어 핀

// 설정 변수
volatile long pulseCount = 0;       // 인터럽트 서비스 루틴(ISR)에서 사용되므로 volatile 선언
const float PPR = 96.0;             // 모터 1회전당 발생하는 펄스 수
unsigned long previousMillis = 0;   // 시간 측정을 위한 기준값
const long interval = 100;

int engine_status = 0;              // 0: 정지, 1: 시동
const int MIN_PWM = 0;              // 모터 성능 최적화 설정
const int POT_MAX_LIMIT = 400;      // 가변저항을 400까지만 돌려도 최대 속도가 나오게 설정

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(7, INPUT_PULLUP); // 7번 핀을 입력으로 설정

  // LCD 및 모터 핀 초기화
  lcd.init();
  lcd.backlight();

  pinMode(dir, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(servoSignalPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  // 엔코더 핀 설정: 내부 풀업 저항을 사용하여 신호를 안정화
  pinMode(encoderPinRed, INPUT_PULLUP);
  pinMode(encoderPinYellow, INPUT_PULLUP);

  // 인터럽트 설정: encoderPinRed(2번 핀)의 신호가 LOW에서 HIGH로 변할 때(RISING)마다 countEncoder 실행
  attachInterrupt(digitalPinToInterrupt(encoderPinRed), countEncoder, RISING);

  digitalWrite(brake, HIGH); // 초기 상태 정지
  digitalWrite(dir, HIGH);
  digitalWrite(servoSignalPin, LOW);
  digitalWrite(relayPin, HIGH); // 릴레이 OFF (전원 차단)
}

void loop() {
  // STM32로부터 시동 상태 수신
  if (mySerial.available() > 0) {
    int val = mySerial.read(); // STM32에서 보낸 값 읽기
    
    // --- 시리얼 모니터 확인용 출력 부분 ---
    Serial.print(">>> Received Data: [ ");
    Serial.print(val); 
    Serial.print(" ] -> ");

    if (val == 0) {                       // Engine OFF 신호
      Serial.println("Engine OFF");
      engine_status = 0;
      digitalWrite(servoSignalPin, LOW);  // 서보 트리거 OFF
      digitalWrite(relayPin, HIGH);        // 릴레이 해제: STM32에 전원 차단
    } 
    else if (val == 1) {                  // Engine ON 신호
      Serial.println("Engine ON");
      engine_status = 1;
      digitalWrite(servoSignalPin, LOW);  // 서보 트리거 OFF
      digitalWrite(relayPin, LOW);       // 릴레이 작동: STM32에 전원 공급 시작
    } 
    else if (val == 2) {
      Serial.println("BRAKE OVER 4000 - SERVO ACTIVE");
      // 시동 상태는 유지하고 서보 신호만 보냄
      digitalWrite(servoSignalPin, HIGH); // 6번 핀으로 다른 STM32에 신호 전송
    } 
    else {
      Serial.println("Unknown Signal");
    }
    // ---------------------------------------
  }

  // 가변저항 값 읽기(0 ~ 1023) 및 모터 속도 결정
  int potValue = analogRead(potPin); 
  int motorSpeed = 0;

  // 가변저항을 50 이상 돌렸을 때만 작동
  if (engine_status == 1 && potValue > 50) { 
    digitalWrite(brake, LOW);  // 모터를 돌리기 위해 브레이크 해제

    // 가변저항의 입력 범위(51~1023)를 모터의 출력 범위(MIN_PWM~255)로 변환
    motorSpeed = map(potValue, 51, POT_MAX_LIMIT, MIN_PWM, 255); 
    motorSpeed = constrain(motorSpeed, 0, 255);
  } 
  else {
    digitalWrite(brake, HIGH); // 저항을 낮추면 즉시 정지 및 브레이크
    motorSpeed = 0;
  }
  analogWrite(pwm, motorSpeed);  // 최종 결정된 속도를 모터 쉴드에 출력

  // 0.1초마다 RPM 계산 및 LCD 출력
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // 펄스 카운트 값을 가져오는 동안 값이 변하지 않도록 인터럽트 잠시 중지
    noInterrupts();
    long pulses = pulseCount;
    pulseCount = 0; // 카운트 초기화
    interrupts();

    float rpm = (pulses / PPR) * 600.0;

    lcd.setCursor(0, 0);
    lcd.print(engine_status ? "ENG:ON " : "ENG:OFF");
    lcd.print(" SPD:"); lcd.print(motorSpeed); lcd.print("   ");

    lcd.setCursor(0, 1);
    lcd.print("RPM: "); lcd.print((int)rpm); lcd.print("       ");

    previousMillis = currentMillis;
  }
}

// 엔코더 인터럽트 서비스 루틴 (ISR)
void countEncoder() {
  // 2번 핀(A상)에 펄스가 들어온 순간 5번 핀(B상)의 상태를 확인하여 방향 판별
  // A상 상승 시점에 B상이 LOW이면 정방향(+), HIGH이면 역방향(-)
  if (digitalRead(encoderPinYellow) == LOW) pulseCount++;
  else pulseCount--;
}