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
#define IDLE_COLLECTER_SIZE 5
#define ACTIVE_COLLECTER_SIZE 50
#define WINDOW_SIZE 5

LIS3DHSettings settings = LIS3DHSettings(4, 10, H, ENABLED, ENABLED, ENABLED);
LIS3DH LIS3DH_Handler = LIS3DH(settings);

using namespace std;

/*
'i' = Idle:
- Active data collector is cleared
- Idle data collector has low frequency
- Changes to data collection upon detecting a start configuration
'a' = Active data collection
- Data is collected at high frequency into the active data collector
- Goes back to idle state if button is pressed
'p' = Processing
- All data collection is paused
'd' = Display
- Corresponding neopixel is turned on
- Goes back to idle if button is pressed
*/
char state = 'i';

float idle_collector[IDLE_COLLECTER_SIZE][4]; // Holds data as tuples { t, {ax, ay, az} }
float idle_jerk_collector[IDLE_COLLECTER_SIZE][3]; // Holds data as tuples { t, {jx, jy, jz} }
int idle_index = 0;

float active_collector[ACTIVE_COLLECTER_SIZE][4]; // Holds data as tuples { t, {ax, ay, az} }
float active_jerk_collector[ACTIVE_COLLECTER_SIZE][3]; // Holds data as tuples { t, {jx, jy, jz} }
int active_index = 0;

float window[WINDOW_SIZE][3] = {{0}};

long last_ms;

int idle_freq = 1;
int active_freq = 10;

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
  //SetupPWMTimer();
  // pinMode(13, OUTPUT);//led indicator when singing a note
  DDRD &= ~(1 << 4);
  DDRC |= (1 << 6);
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
  last_ms = millis();
}

bool collect(float acceleration_collector[][4], int& index, float jerk_collector[][3], int frequency, long& last_ms) {
  if (millis() - last_ms >= 1000.0 / frequency) {
    last_ms = millis();
    acceleration_collector[index][0] = (float) last_ms / (float) 1000;
    acceleration_collector[index][1] = LIS3DH_Handler.getXFloat_SI();
    acceleration_collector[index][2] = LIS3DH_Handler.getYFloat_SI();
    acceleration_collector[index][3] = LIS3DH_Handler.getZFloat_SI();
    if (index > 0) {
      jerk_collector[index - 1][0] = (acceleration_collector[index][1] - acceleration_collector[index - 1][1]) * 1000.0 / (last_ms - acceleration_collector[index - 1][0]);
      jerk_collector[index - 1][1] = (acceleration_collector[index][2] - acceleration_collector[index - 1][2]) * 1000.0 / (last_ms - acceleration_collector[index - 1][0]);
      jerk_collector[index - 1][2] = (acceleration_collector[index][3] - acceleration_collector[index - 1][3]) * 1000.0 / (last_ms - acceleration_collector[index - 1][0]); 
    }
    index++;
    return true;
  }
  return false;
}

bool check_start() {
  bool res = true;
  for (int i = 0; i < 5; i++) {
    res &= (abs(idle_jerk_collector[idle_index - (i+1)][0]) < 0.5) && (abs(idle_jerk_collector[idle_index - (i+1)][1]) < 0.5) && (abs(idle_jerk_collector[idle_index - (i+1)][2]) < 0.5);
  }
  Serial.println("----------");
  Serial.println(res);
  if (true) {
    for (int i = 0; i < 5; i++) {
      Serial.print("(");
      Serial.print(idle_jerk_collector[idle_index - (i + 1)][0]);
      Serial.print(", ");
      Serial.print(idle_jerk_collector[idle_index - (i + 1)][1]);
      Serial.print(", ");
      Serial.print(idle_jerk_collector[idle_index - (i + 1)][2]);
      Serial.print(")");
      Serial.println();
    }
  }
  Serial.println("----------");
  return res;
}

void flush(float acceleration_collector[][4], int& index, float jerk_collector[][3]) {
  for (int i = 0; i < index; i++) {
    for (int j = 0; j < 3; j++) {
      acceleration_collector[i][j] = 0;
      jerk_collector[i][j] = 0;
    }
    acceleration_collector[i][3] = 0;
  }
  index = 0;
}

bool just_added = false;

void loop() {
  if (((PIND >> 4) & 1)) {
    state = 'i';
    flush(idle_collector, idle_index, idle_jerk_collector);
  }
  switch (state) {
    // IDLE
    case 'i': {
      if (idle_index < IDLE_COLLECTER_SIZE) {
        just_added = collect(idle_collector, idle_index, idle_jerk_collector, idle_freq, last_ms);
        if (just_added && idle_index >= 5) {
          if (check_start()) {
            state = 'a';
          }
        }
      }
      else {
        flush(idle_collector, idle_index, idle_jerk_collector);
      }
    }
    break;
    // ACTIVE DATA COLLECTION
    case 'a': {
      if (active_index < ACTIVE_COLLECTER_SIZE) {
        collect(active_collector, active_index, active_jerk_collector, active_freq, last_ms);
      }
    }
    break;
    // PROCESSING
    case 'p': {

    }
    break;
    // DISPLAY
    case 'd': {
      
    }
    break;
  }
}