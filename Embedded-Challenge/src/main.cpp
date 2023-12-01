#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif
#ifndef GESTURES
#define GESTURES 0 
#include "gestures.h"
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
#define NO_MOTION_TIME 3
#define MAX_GESTURE_LEN 3
#define WINDOW_SIZE 5
#define NO_MOTION_THRESHOLD 0.75
#define NUM_GESTURES 10

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

long subtotals[3] = {0};
int window_index = 0;

long prev_last_ms;
long last_ms;

float average_time_diff = 0;
uint32_t count_ticks = 0;

const uint8_t idle_freq = 2;
const uint8_t active_freq = 5;

const uint8_t collecter_size = active_freq * MAX_GESTURE_LEN;

int16_t collecter[collecter_size][3] {{0}}; // Holds data { ax, ay, az }
int collecter_index = 0;

uint32_t DWT_matrix[collecter_size + 1][collecter_size + 1] = {{(uint32_t) 0xffffffff}};

uint8_t wait_between_checks = 0;

void setup() {
  DDRC |= (1 << 7);
  DDRC |= (1 << 6);
  DDRF &= ~(1 << 6); // Right Button
  DDRD &= ~(1 << 4); // Left Button
  Serial.begin(9600);
  SPI_MasterInit();
  LIS3DH_Handler = LIS3DH(settings);
  LIS3DH_Handler.SetupAccelerometer();
  sing((Song) MARIO);
  last_ms = millis();
}

void calculate_DWT(const uint16_t* gesture) {
  DWT_matrix[0][0] = 0;
  for (int r = 1; r < collecter_size + 1; r++) {
    for (int c = 1; c < collecter_size + 1; c++) {
      DWT_matrix[r][c] = abs(gesture[r * 3 + 0] - collecter[c][0]) + abs(gesture[r * 3 + 1] - collecter[c][1]) + abs(gesture[r * 3 + 2] - collecter[c][2]) + min(DWT_matrix[r - 1][c - 1], min(DWT_matrix[r - 1][c], DWT_matrix[r][c - 1]));
    }
  }
}

bool collect(uint8_t frequency) {
  if (millis() - last_ms >= 1000.0 / (frequency * WINDOW_SIZE)) {
    prev_last_ms = last_ms;
    last_ms = millis();
    subtotals[0] += LIS3DH_Handler.getXRaw();
    subtotals[1] += LIS3DH_Handler.getYRaw();
    subtotals[2] += LIS3DH_Handler.getZRaw();
    window_index++;
  }
  if (window_index == WINDOW_SIZE) {
    average_time_diff = (average_time_diff * count_ticks + (float) (last_ms - prev_last_ms) * (float) WINDOW_SIZE / 1000.0) / (count_ticks + 1);
    count_ticks ++;
    collecter[collecter_index][0] = subtotals[0] / window_index ;
    collecter[collecter_index][1] = subtotals[1] / window_index ;
    collecter[collecter_index][2] = subtotals[2] / window_index ;
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
  // Serial.println("------------");
  for (int i = 1; i < stop; i++) {
    float factor = settings.Calc_Div_Factor();
    float idle_jerk_x = (collecter[collecter_index - stop + i][0] - collecter[collecter_index - stop + (i - 1)][0]) / (factor * average_time_diff);
    float idle_jerk_y = (collecter[collecter_index - stop + i][1] - collecter[collecter_index - stop + (i - 1)][1]) / (factor * average_time_diff);
    float idle_jerk_z = (collecter[collecter_index - stop + i][2] - collecter[collecter_index - stop + (i - 1)][2]) / (factor * average_time_diff);
    res &= (abs(idle_jerk_x) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_y) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_z) < NO_MOTION_THRESHOLD);
  }
  // Serial.println("------------");
  wait_between_checks = 0;
  return res;
}

void flush(int16_t acceleration_collector[][3], int& index) {
  for (int i = 0; i < index; i++) {
    for (int j = 0; j < 3; j++) {
      acceleration_collector[i][j] = 0;
    }
  }
  index = 0;
  count_ticks = 0;
  average_time_diff = 0;
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
      if (collecter_index < collecter_size) {
        just_added = collect(idle_freq);
        if (just_added) {
          wait_between_checks++;
          if (wait_between_checks >= NO_MOTION_TIME * idle_freq) {
            if (check_start()) {
              state = 'a';
              flush(collecter, collecter_index);
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
      if (collecter_index < collecter_size) {
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
      // for (int i = 0; i < NUM_GESTURES; i++) {
      //   calculate_DWT(gestures[i]);
      // }
    }
    break;
    // DISPLAY
    case 'd': {
      
    }
    break;
  }
}