// Importing whatever hasn't already been imported

#ifndef PREDIRECTIVES
#include "predirectives.h"
#define PREDIRECTIVES 0
#endif
#ifndef GESTURES
#define GESTURES 0 
#include "gestures.h"
#endif
#ifndef SPI_OWN
#include "SPI_Own.h"
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

// Defining relevant global constants

#define NO_MOTION_TIME 3
#define MAX_GESTURE_LEN 2.99
#define WINDOW_SIZE 5
#define NO_MOTION_THRESHOLD 0.15
#define NUM_GESTURES 10
#define NUM_TRIALS 2
#define IDLE_FREQ 2
#define ACTIVE_FREQ 7
#include "Copied_Adafruit.h"

using namespace std;

/*
  Accelerometer handling objects.
  The chosen accelerometer settings are:
  - Max/Min acceleration = +/- 4g
  - Frequency 10 Hz (acceleration is measured every 100 ms)
  - High Power Mode to allow for higher precision in reading the data
  - All 3 channels ENABLED
*/
LIS3DHSettings settings = LIS3DHSettings(4, 10, (PM) H, ENABLED, ENABLED, ENABLED);
LIS3DH LIS3DH_Handler = LIS3DH(settings);

/*
'i' = Idle (Lasts as long as the user doesn't stay still)
- Data collector has low frequency (IDLE_FREQ)
- Changes to data collection upon detecting a start configuration (holding still for NO_MOTION_TIME seconds)
'a' = Active data collection (Lasts MAX_GESTURE_LEN seconds or until the user reverts the board back to the idle frequency mode)
- Data is collected at high frequency (ACTIVE_FREQ) into the data collector
- Goes back to idle state if (left/right) button is pressed *******
'p' = Processing (Lasts as many seconds as it takes to perform the algorithm to run)
- All data collection is paused
- DTW algorithm runs to compute the distances of the collected data from each of the prior recordings
- The minimum distance decides the chosen gesture
'd' = Display (Lasts until user presees the button)
- Corresponding neopixel is turned on
- Goes back to idle if (left/right) button is pressed ******
*/
char state = 'i';

/*
  Moving average filter setup
  subtotals holds the cummulitive ax, ay, and az, sums to create a moving average filter
  window_index counts up until WINDOW_SIZE before averaging the sum
*/ 
long subtotals[3] = {0};
int window_index = 0;

long prev_last_ms;
long last_ms;

/*
  Calculates the average time between the acceleration recordings to produce the denominator of the jerk calculation.
  Additionally useful for debugging and making sure the board is recording at the desired frequency
*/
float average_time_diff = 0;
uint32_t count_ticks = 0;

const uint8_t collecter_size = ACTIVE_FREQ * MAX_GESTURE_LEN;

/*
  The data collecter holds the recorded accelerations
*/
int16_t collecter[collecter_size][3] {{0}}; // Holds data { ax, ay, az }
int collecter_index = 0;

/*
  The DTW matrix is used to compute the DTW distance between the collected data and the previous data
*/
float DTW_matrix[collecter_size + 1][collecter_size + 1] {{INFINITY}};

/// @brief Adds a wait between checking whent he start condition has started to ensure the code doesn't spend most of the time checking the start condition
uint8_t wait_between_checks = 0;

/// @brief Holds the chosen gesture as a result of the DTW algorithm
uint8_t chosen_gesture;


/// @brief Sets up all functionalities before entering the loop
void setup() {
  // Setting up the neopizels
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(20);
  CircuitPlayground.clearPixels();

  DDRC |= (1 << 7); // Setting up the pin connected to the speaker as output
  DDRC |= (1 << 6); // Setting up the pin connected to the right LED as output
  DDRF &= ~(1 << 6); // Right Button
  DDRD &= ~(1 << 4); // Left Button
  
  Serial.begin(9600); // Setting up the serial connection

  // Setting up the accelerometer
  SPI_MasterInit(); // First setting up the SPI connection
  LIS3DH_Handler = LIS3DH(settings); // Initializing the LIS3DH handler with the chosen settings
  LIS3DH_Handler.SetupAccelerometer(); // Setting up the accelerometer

  // Setting up the DTW matrix to hold infinity in the beginning
  for (int r = collecter_size; r >= 0; r--) {
    for (int c = 0; c < collecter_size; c++) {
      DTW_matrix[r][c] = INFINITY;
    }
  }

  sing((Song) MARIO); // By the end of the song, everything is ready and the idle state begins
  
  last_ms = millis(); // Recording the current time to calculate the change in time later
}

/*
  Performs the DTW algorithm and returns the computed distance. The Domain Time Warping (DTW) algorithm attempts to find the best mapping
  between the points of one time series and another. The algorithm calculates all possible mappings at each step, assumes the minimum, 
  and proceeds to do that again with that assumption in mind. By the end of the algorithm, the minimum distance is going to be held in the
  top right entry of the matrix.
*/
float calculate_DTW(const int16_t* gesture) {
  DTW_matrix[0][0] = 0;
  for (int r = 1; r < collecter_size + 1; r++) {
    for (int c = 1; c < collecter_size + 1; c++) {
      float dist = sqrt(pow((int16_t)pgm_read_word(gesture + (r - 1) * 3 + 0) - (float)collecter[(c-1)][0], 2) + pow((int16_t)pgm_read_word(gesture + (r - 1) * 3 + 1) - (float)collecter[(c-1)][1], 2) + pow( (int16_t)pgm_read_word(gesture + (r - 1) * 3 + 2) - (float) collecter[(c-1)][2], 2) );
      DTW_matrix[r][c] = abs(dist + min(DTW_matrix[r - 1][c - 1], min(DTW_matrix[r - 1][c], DTW_matrix[r][c - 1])));
    }
  }
  return DTW_matrix[collecter_size][collecter_size];
}

/*
  This function takes in a frequency, and according to the last time data was collected, collects more data
  into the average calculator buffer. Once enough data points have been compounded in the buffer, the
  average is computed and placed in the main collector array. The implementation of the moving average
  filter is slightly different from what we implemented in class but the result is still the same.
*/
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
    // The serial commands below are important when the data is collected for the first time and can be removed later
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

/*
  The start condition is that the board stays stationary for NO_MOTION_TIME seconds consecutively. To check that, the
  jerk, the rate of change of acceleration, is computed, and if it is really close to 0 (less than NO_MOTION_THRESHOLD
  which was chosen experimentally), the function returns that the start condition was met. This is much faster than
  computing the DTW distance relative to a previous recording of no motion, and much more accurate.
*/
bool check_start() {
  bool res = true;
  int stop = NO_MOTION_TIME * IDLE_FREQ;
  for (int i = 1; i < stop; i++) {
    float factor = settings.Calc_Div_Factor();
    float idle_jerk_x = (collecter[collecter_index - stop + i][0] - collecter[collecter_index - stop + (i - 1)][0]) / (9.8 * factor * average_time_diff);
    float idle_jerk_y = (collecter[collecter_index - stop + i][1] - collecter[collecter_index - stop + (i - 1)][1]) / (9.8 * factor * average_time_diff);
    float idle_jerk_z = (collecter[collecter_index - stop + i][2] - collecter[collecter_index - stop + (i - 1)][2]) / (9.8 * factor * average_time_diff);
    res &= (abs(idle_jerk_x) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_y) < NO_MOTION_THRESHOLD) && (abs(idle_jerk_z) < NO_MOTION_THRESHOLD);
  }
  wait_between_checks = 0;
  return res;
}

//  Flushes the acceleration collector and resets the index
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
  // Check if left button is pressed and the program is currently in the active state
  if (((PIND >> 4) & 1) && state == 'a') {
    // Go to the idle state and clear the collector
    state = 'i';
    CircuitPlayground.clearPixels();
    flush(collecter, collecter_index, collecter_size);
    sing((Song) PROCESSING); // Signifies end of the state change
  }
  // if (state == 'a' && ((PINF >> 6) & 1)) {
  //   state = 'p';
  //   CircuitPlayground.clearPixels();
  //   sing((Song) PROCESSING);
  // }
  // Check if right button is pressed and the program is currently in the display state
  if (state == 'd' && ((PINF >> 6) & 1)) { 
    // Go back to the idle state and clear the collecter
    state = 'i';
    CircuitPlayground.clearPixels();
    flush(collecter, collecter_index, collecter_size);
    sing((Song) PROCESSING); // Signifies the end of the state change
  }

  // Switch statement to handle the behaviour at each state
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
        CircuitPlayground.clearPixels();
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
      float min = INFINITY;
      for (int i = 0; i < NUM_GESTURES * NUM_TRIALS; i++) {
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
        Serial.println(char(pgm_read_byte(gesture_names + (chosen_gesture % NUM_GESTURES))));
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(chosen_gesture % NUM_GESTURES, 128, 50, 30);
        flush(collecter, collecter_index, collecter_size);
        Serial.println(F("-------------"));
        just_added = false;
      }
    }
    break;
  }
}