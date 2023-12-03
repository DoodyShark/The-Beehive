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
#define MAX_GESTURE_LEN 2.9
#define WINDOW_SIZE 5
#define NO_MOTION_THRESHOLD 0.25
#define NUM_GESTURES 5
#define IDLE_FREQ 2
#define ACTIVE_FREQ 7

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
uint8_t chosen_gesture;

long subtotals[3] = {0};
int window_index = 0;

long prev_last_ms;
long last_ms;

float average_time_diff = 0;
uint32_t count_ticks = 0;

const uint8_t collecter_size = ACTIVE_FREQ * MAX_GESTURE_LEN;

int16_t collecter[collecter_size][3] {{0}}; // Holds data { ax, ay, az }
int collecter_index = 0;

float DTW_matrix[collecter_size + 1][collecter_size + 1] {{INFINITY}};

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
  for (int r = collecter_size; r >= 0; r--) {
    for (int c = 0; c < collecter_size; c++) {
      DTW_matrix[r][c] = INFINITY;
    }
  }
  // for (int r = collecter_size; r >= 0; r--) {
  //   for (int c = 0; c < collecter_size; c++) {
  //     Serial.print(DTW_matrix[r][c]);
  //     Serial.print('\t');
  //   }
  //   Serial.println();
  // }
  // Serial.println();
  // Serial.println();
  last_ms = millis();
}

float calculate_DTW(const int16_t* gesture) {
  DTW_matrix[0][0] = 0;
  for (int r = 1; r < collecter_size + 1; r++) {
    // Serial.print("(");
    // Serial.print((int16_t)pgm_read_word(gesture + (r-1) * 3 + 0));
    // Serial.print(", ");
    // Serial.print((int16_t)pgm_read_word(gesture + (r-1) * 3 + 1));
    // Serial.print(", ");
    // Serial.print((int16_t)pgm_read_word(gesture + (r-1) * 3 + 2));
    // Serial.print(")");
    // Serial.println();
    for (int c = 1; c < collecter_size + 1; c++) {
      float dist = sqrt(pow((int16_t)pgm_read_word(gesture + (r - 1) * 3 + 0) - (float)collecter[(c-1)][0], 2) + pow((int16_t)pgm_read_word(gesture + (r - 1) * 3 + 1) - (float)collecter[(c-1)][1], 2) + pow( (int16_t)pgm_read_word(gesture + (r - 1) * 3 + 2) - (float) collecter[(c-1)][2], 2) );
      // Serial.println(dist);
      DTW_matrix[r][c] = abs(dist + min(DTW_matrix[r - 1][c - 1], min(DTW_matrix[r - 1][c], DTW_matrix[r][c - 1])));
      // Serial.println(DTW_matrix[r][c]);
    }
  }
  // for (int r = collecter_size; r >= 0; r--) {
  //   for (int c = 0; c < collecter_size; c++) {
  //     Serial.print(DTW_matrix[r][c]);
  //     Serial.print('\t');
  //   }
  //   Serial.println();
  // }
  // Serial.println();
  // Serial.println();
  return DTW_matrix[collecter_size][collecter_size];
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
    Serial.print(collecter[collecter_index][0]);
    Serial.print(F(", "));
    Serial.print(collecter[collecter_index][1]);
    Serial.print(F(", "));
    Serial.print(collecter[collecter_index][2]);
    Serial.print(F(",\n"));
    collecter_index++;
    window_index = 0;
    subtotals[0] = subtotals[1] = subtotals[2] = 0;
    return true;
  }
  return false;
}

bool check_start() {
  bool res = true;
  int stop = NO_MOTION_TIME * IDLE_FREQ;
  // Serial.println("------------");
  for (int i = 1; i < stop; i++) {
    float factor = settings.Calc_Div_Factor();
    float idle_jerk_x = (collecter[collecter_index - stop + i][0] - collecter[collecter_index - stop + (i - 1)][0]) / (9.8 * factor * average_time_diff);
    float idle_jerk_y = (collecter[collecter_index - stop + i][1] - collecter[collecter_index - stop + (i - 1)][1]) / (9.8 * factor * average_time_diff);
    float idle_jerk_z = (collecter[collecter_index - stop + i][2] - collecter[collecter_index - stop + (i - 1)][2]) / (9.8 * factor * average_time_diff);
    res &= (abs(idle_jerk_x) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_y) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_z) < NO_MOTION_THRESHOLD);
  }
  // Serial.println("------------");
  wait_between_checks = 0;
  return res;
}

void flush(int16_t acceleration_collector[][3], int& index, int size) {
  for (int i = 0; i < size; i++) {
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
    flush(collecter, collecter_index, collecter_size);
  }
  if (state == 'a' && ((PINF >> 6) & 1)) {
    state = 'p';
    sing((Song) PROCESSING);
  }
  if (state == 'd' && ((PINF >> 6) & 1)) { 
    state = 'i';
    sing((Song) PROCESSING);
  }
  switch (state) {
    // IDLE
    case 'i': {
      if (collecter_index < collecter_size) {
        just_added = collect(IDLE_FREQ);
        if (just_added) {
          wait_between_checks++;
          if (wait_between_checks >= NO_MOTION_TIME * IDLE_FREQ) {
            if (check_start()) {
              state = 'a';
              flush(collecter, collecter_index, collecter_size);
              sing((Song) START);
              Serial.println(F("---------------"));
            }
          }
        }
      }
      else {
        flush(collecter, collecter_index, collecter_size);
      }
    }
    break;
    // ACTIVE DATA COLLECTION
    case 'a': {
      if (collecter_index < collecter_size) {
        collect(ACTIVE_FREQ);
      }
      else {
        state = 'p';
        sing((Song) PROCESSING);
      }
    }
    break;
    // PROCESSING
    case 'p': {
      Serial.println(F("-------------"));
      uint32_t min = UINT32_MAX;
      for (int i = 0; i < NUM_GESTURES; i++) {
        float curr = calculate_DTW(gestures[i]);
        Serial.println(curr);
        if (curr < min) {
          min = curr;
          chosen_gesture = i;
        }
      }
      state = 'd';
      just_added = true;
    }
    break;
    // DISPLAY
    case 'd': {
      if (just_added) {
        Serial.print(F("Chose: "));
        Serial.println(char(pgm_read_byte(gesture_names + chosen_gesture)));
        flush(collecter, collecter_index, collecter_size);
        Serial.println(F("-------------"));
        just_added = false;
      }
    }
    break;
  }
}