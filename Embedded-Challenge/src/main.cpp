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

LIS3DHSettings settings = LIS3DHSettings(4, 10, H, ENABLED, ENABLED, ENABLED);
LIS3DH LIS3DH_Handler = LIS3DH(settings);

void setup() {
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
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