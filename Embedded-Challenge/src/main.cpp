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

using namespace LIS3DH_Own::Setup;
using namespace LIS3DH_Own::Data;

LIS3DHSettings settings = LIS3DHSettings(2, 10, 'l', true, true, true);

void setup() {
  Serial.begin(9600);
  SPI_MasterInit();
  SetupAccelerometer(settings);
}

void loop() {
  delay(100);
  float dataX = getXFloat(settings);
  float dataY = getYFloat(settings);
  float dataZ = getZFloat(settings);

  int16_t dataX_raw = getXRaw();
  int16_t dataY_raw = getYRaw();
  int16_t dataZ_raw = getZRaw();
  
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