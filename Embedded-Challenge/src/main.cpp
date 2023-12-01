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
#define NO_MOTION_TIME 5
#define COLLECTER_SIZE 100
#define WINDOW_SIZE 5
#define NO_MOTION_THRESHOLD 0.75

LIS3DHSettings settings = LIS3DHSettings(4, 10, (PM) H, ENABLED, ENABLED, ENABLED);
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

float collecter[COLLECTER_SIZE][4] {{0}}; // Holds data { t, ax, ay, az }
int collecter_index = 0;

float subtotals[3] = {0};
int window_index = 0;

long last_ms;

int idle_freq = 1;
int active_freq = 10;

int wait_between_checks = 0;

void setup() {
  DDRC |= (1 << 7);
  DDRC |= (1 << 6);
  DDRF &= ~(1 << 6);
  DDRD &= ~(1 << 4);
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
  last_ms = millis();
}

bool collect(int frequency) {
  if (millis() - last_ms >= 1000.0 / (frequency * WINDOW_SIZE)) {
    subtotals[0] += LIS3DH_Handler.getXFloat_SI();
    subtotals[1] += LIS3DH_Handler.getYFloat_SI();
    subtotals[2] += LIS3DH_Handler.getZFloat_SI();
    window_index++;
  }
  if (window_index == 5) {
    last_ms = millis();
    collecter[collecter_index][0] = (float) last_ms / (float) 1000;
    collecter[collecter_index][1] = subtotals[0] / window_index ;
    collecter[collecter_index][2] = subtotals[1] / window_index ;
    collecter[collecter_index][3] = subtotals[2] / window_index ;
    // Serial.print("(");
    // Serial.print(collecter[collecter_index][0]);
    // Serial.print(", ");
    // Serial.print(collecter[collecter_index][1]);
    // Serial.print(", ");
    // Serial.print(collecter[collecter_index][2]);
    // Serial.print(")");
    collecter_index++;
    window_index = 0;
    subtotals[0] = subtotals[1] = subtotals[2] = 0;
    return true;
  }
  return false;
}

bool check_start() {
  bool res = true;
  int stop = NO_MOTION_TIME * idle_freq;
  for (int i = 1; i < stop; i++) {
    float idle_jerk_x = (collecter[collecter_index - 5 + i][1] - collecter[collecter_index - 5 + (i - 1)][1]) / (collecter[collecter_index - 5 + i][0] - collecter[collecter_index - 5 + (i - 1)][0]);
    float idle_jerk_y = (collecter[collecter_index - 5 + i][2] - collecter[collecter_index - 5 + (i - 1)][2]) / (collecter[collecter_index - 5 + i][0] - collecter[collecter_index - 5 + (i - 1)][0]);
    float idle_jerk_z = (collecter[collecter_index - 5 + i][3] - collecter[collecter_index - 5 + (i - 1)][3]) / (collecter[collecter_index - 5 + i][0] - collecter[collecter_index - 5 + (i - 1)][0]);
    res &= (abs(idle_jerk_x) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_y) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_z) < NO_MOTION_THRESHOLD);
  }
  wait_between_checks = 0;
  return res;
}

void flush(float acceleration_collector[][4], int& index) {
  for (int i = 0; i < index; i++) {
    for (int j = 0; j < 4; j++) {
      acceleration_collector[i][j] = 0;
    }
  }
  index = 0;
}

bool just_added = false;

void loop() {
  if (((PIND >> 4) & 1)) {
    state = 'i';
    flush(collecter, collecter_index);
  }
  if (state == 'a' && ((PINF >> 6) & 1)) {
    state = 'p';
    sing((Song) PROCESSING);
  }
  switch (state) {
    // IDLE
    case 'i': {
      if (collecter_index < COLLECTER_SIZE) {
        just_added = collect(idle_freq);
        if (just_added) {
          wait_between_checks++;
          if (wait_between_checks >= NO_MOTION_TIME * idle_freq) {
            if (check_start()) {
              state = 'a';
              sing((Song) START);
            }
          }
        }
      }
      else {
        flush(collecter, collecter_index);
      }
    }
    break;
    // ACTIVE DATA COLLECTION
    case 'a': {
      if (collecter_index < COLLECTER_SIZE) {
        collect(active_freq);
      }
      else {
        state = 'p';
        sing((Song) PROCESSING);
      }
    }
    break;
    // PROCESSING
    case 'p': {
      Serial.println("Processing");
      while (true);
    }
    break;
    // DISPLAY
    case 'd': {
      
    }
    break;
  }
}