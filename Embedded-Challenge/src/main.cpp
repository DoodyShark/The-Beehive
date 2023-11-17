#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif
#ifndef SPI_OWN
#include "SPI_Own.h"
#define SPI_OWN 0
#endif
#ifndef LIS3DH_OWN
#include "LIS3DH_Own.h"
#define LIS3DH_OWN 0
#endif

// using namespace LIS3DH_Own::Setup;
// using namespace LIS3DH_Own::Data;

LIS3DHSettings settings = LIS3DHSettings(4, 10, 'l', true, true, true);
LIS3DH LIS3DH_Handler = LIS3DH(settings);

void setup() {
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
}

void loop() {
  delay(100);
  float dataX = LIS3DH_Handler.getXFloat();
  float dataY = LIS3DH_Handler.getYFloat();
  float dataZ = LIS3DH_Handler.getZFloat();

  int16_t dataX_raw = LIS3DH_Handler.getXRaw();
  int16_t dataY_raw = LIS3DH_Handler.getYRaw();
  int16_t dataZ_raw = LIS3DH_Handler.getZRaw();
  
  Serial.print("At time ");
  Serial.print(millis());
  Serial.print(" (");
  Serial.print(dataX_raw);
  Serial.print(", ");
  Serial.print(dataY_raw);
  Serial.print(", ");
  Serial.print(dataZ_raw);
  Serial.print(") -> (");
  Serial.print(dataX);
  Serial.print(", ");
  Serial.print(dataY);
  Serial.print(", ");
  Serial.print(dataZ);
  Serial.println(")");
}