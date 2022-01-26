#include "GyverPower.h"

const byte DOORS[] = {A0,A1,A2,A3};
const byte OPERATING[] = {4,5};

const int unsigned CHECK = 20;
const int unsigned SLEEP_PIN = 3;
const int unsigned TRIGGER_VALUE = 1000;
//const unsigned long CALIBRATION_VAL = 35000;
const unsigned long CALIBRATION_VAL = 328000;

void setup() {
  for (byte i = 0; i < sizeof(DOORS); i++) {
    pinMode(DOORS[i], OUTPUT);
    digitalWrite(DOORS[i], HIGH);
  }

  for (byte i = 0; i < sizeof(OPERATING); i++) {
    pinMode(OPERATING[i], OUTPUT);
    digitalWrite(OPERATING[i], LOW);
  }

//  // кнопка подключена к GND и D3
//  pinMode(3, INPUT_PULLUP);
//
//  // подключаем прерывание на пин D3 (Arduino NANO)
//  attachInterrupt(1, isr, FALLING);

//  Serial.begin(9600);
}

bool calibration = true;

void loop() {
  volatile unsigned long cVal = 0;
  uint32_t a = millis();

  if (calibration) digitalWrite(13, HIGH); // calibration on
  else digitalWrite(13, LOW); // calibration off

  while (calibration) { // calibration handle
    delay(5);
    cVal = cVal + analogRead(DOORS[0]);
    if (cVal >= CALIBRATION_VAL) calibration = false;
//    if (millis() - a > 2000) Serial.println(cVal);
  }

  for (byte i = 0; i < sizeof(DOORS); i++) { // handle doors
    int unsigned val = analogRead(DOORS[i]);
    if (val < TRIGGER_VALUE) handle(DOORS[i]);
  }
}

void handle(int pin) {
  uint32_t firstDown = millis();
  
  unsigned int count = 1;
  unsigned int lastCount = 1;
  
  uint32_t lastUp;
  uint32_t firstUp;
  uint32_t lastDown;

  do {
    int impuls = analogRead(pin);

    if (impuls > TRIGGER_VALUE && !firstUp) firstUp = millis();
    if (impuls > TRIGGER_VALUE && firstUp) lastUp = millis();

    if (impuls < TRIGGER_VALUE) {
      count = count + 1;
      firstUp = 0;
      lastUp = 0;
    }

    if (impuls > TRIGGER_VALUE) {
      int val = (count + lastCount) / 2;

      if (count > 0) {
        if (val > 50 && val < 110) open();
        else if (val > 150 && val < 350) close();

        lastCount = count;
      }

      count = 0;
    }
  } while ((lastUp - firstUp) < CHECK);

  for (byte i = 0; i < sizeof(OPERATING); i++) {
    digitalWrite(OPERATING[i], LOW);
  }
}

void open() {
  digitalWrite(OPERATING[1], LOW);
  digitalWrite(OPERATING[0], HIGH);
}

void close() {
  digitalWrite(OPERATING[0], LOW);
  digitalWrite(OPERATING[1], HIGH);
}

// обработчик аппаратного прерывания
void isr() {
  // дёргаем за функцию "проснуться"
  // без неё проснёмся чуть позже (через 0-8 секунд)
  // power.wakeUp();
  Serial.println(111);
}
