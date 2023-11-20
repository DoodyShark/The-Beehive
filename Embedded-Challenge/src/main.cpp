#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif
#ifndef SPI_OWN
#include "SPI.h"
#define SPI_OWN 0
#endif
#ifndef LIS3DH_OWN
#include "LIS3DH.h"
#define LIS3DH_OWN 0
#endif
#ifndef SPEAKER
#define SPEAKER 0
#include "speaker.h"
#endif

LIS3DHSettings settings = LIS3DHSettings(4, 10, H, ENABLED, ENABLED, ENABLED);
LIS3DH LIS3DH_Handler = LIS3DH(settings);


void SetupPWMTimer() {
  // SETTING UP TIMER1 TO PRODUCE OUTPUT AT OC1B (PB6)
  // WGM13-WGM10 = 0111 for fast PWM with 0x03ff as TOP:
  TCCR3B &= ~(1 << WGM33);
  TCCR3B &= ~(1 << WGM32);
  TCCR3A |= (1 << WGM31);
  TCCR3A |= (1 << WGM30);
  
  // COM3A1-0 = 10 For clearing OC3A at compare match, setting at TOP
  TCCR3A |= (1 << 7);
  TCCR3A &= ~(1 << 6);
  // CSn2-0 = 011 for clkI/O prescaling by 64
  TCCR3B &= ~(1 << 2);
  TCCR3B |= (1 << 1);
  TCCR3B &= ~(1 << 0);
  // Setting up OCR3A
  OCR3AH = (uint8_t) 0x00;
  OCR3AL = (uint8_t) 0x00;
  // Setting up PC6 in output mode to produce OC1A:
  DDRC |= (1 << 6);
}


void setup() {
  SetupPWMTimer();
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
  sing(1);
  sing(2);
}

void loop() {
  delay(100);
  float dataX = LIS3DH_Handler.getXFloat_SI();
  float dataY = LIS3DH_Handler.getYFloat_SI();
  float dataZ = LIS3DH_Handler.getZFloat_SI();

  // int16_t dataX_raw = LIS3DH_Handler.getXRaw();
  // int16_t dataY_raw = LIS3DH_Handler.getYRaw();
  // int16_t dataZ_raw = LIS3DH_Handler.getZRaw();
  
  Serial.print("At time ");
  Serial.print(millis());
  Serial.print(": (");
  // Serial.print(dataX_raw);
  // Serial.print(", ");
  // Serial.print(dataY_raw);
  // Serial.print(", ");
  // Serial.print(dataZ_raw);
  // Serial.print(") -> (");
  Serial.print(dataX, 2);
  Serial.print(", ");
  Serial.print(dataY, 2);
  Serial.print(", ");
  Serial.print(dataZ, 2);
  Serial.println(")");
}