#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_CircuitPlayground.h>

// Brief explanation on board usage:
// - Press the right button on the board to start recording your gesture
// - Press the left button on the board to enter/check your gesture in order to unlock the board
// - When in recording mode, the small on board LED (connected to digital pin 13) will be lit up 
//   to indicate the board is in recording mode
// - After entering the gesture, the on board neo pixels will be lit up to indicate the check result.
//   If the neo pixels are in red color and an incorrect sound effect is played, then the entered gesture 
//   is incorrect. If the neo pixels are in green color and a correct sound effect is played by the on 
//   board speaker, then the entered gesture is correct.
// - You can also open serial monitor to check current mode (record or enter), check result (correct or incorrect),
//   and recorded values

// Pin number initialization
#define recordPin 19 // right button as record button
#define enterPin 4 // left button as enter button
#define LEDPin 13 // on baord LED
#define NeoPin 17 // on board neo pixel 
#define NeoCount 10 // total number of neo pixel used
#define speakerPin 31 // on board speaker 

// Neo Pixel initialization & color declaration 
Adafruit_NeoPixel strip(NeoCount, NeoPin, NEO_GRB + NEO_KHZ800); // neo pixel object declaration 
uint32_t red = strip.Color(255, 0, 0); // color red
uint32_t green = strip.Color(0, 255, 0); // color green
uint32_t magenta = strip.Color(255, 0, 255); // color magenta
uint32_t greenishwhite = strip.Color(0, 64, 0, 64); // color greenishwhite

// Variable initialization
uint8_t recordButton = 0; // this is record key state
uint8_t enterButton = 0; // this is enter key state
bool recording = false;  // if user is recording 
bool entering = false; // if user is entering
uint8_t recordIndex = 0; // for keeping track of how many values are recorded and for array index
uint8_t enterIndex = 0; // kepp track of how many entered values checked
uint8_t count = 0; // keep track of how many recorded values are out of range
bool check = false; // if the recorded value is out of range
bool wraparound = false; // if tolerance range is computed using wraparound
uint8_t toleranceCount = 10; // number of tolerable out of range values 
uint8_t toleranceRange = 15; // tolerance range, set the upper and lower limit 
uint8_t x[150]; // for storing x values
uint8_t y[150]; // for storing y values
uint8_t z[150]; // for storing z values
uint8_t xUpper; // x upper tolerance
uint8_t xLower; // x lower tolerance
uint8_t yUpper; // y upper tolerance 
uint8_t yLower; // y lower tolerance
uint8_t zUpper; // z upper tolerance
uint8_t zLower; // z lower tolerance

// Check if value is out of range in normal range
void checkValue(uint8_t X, uint8_t Y, uint8_t Z) {
  if ((X > xUpper) || (X < xLower)) { // x out of range
    check = true;
  }
  if ((Y > yUpper) || (Y < yLower)) { // y out of range
    check = true;
  }
  if ((Z > zUpper) || (Z < zLower)) { // z out of range
    check = true;
  }
}

// Check if value is out of range in wraparound range
void checkWraparoundValue(uint8_t X, uint8_t Y, uint8_t Z) {
  if ((X > xUpper) && (X < xLower)) { // x out of range
    check = true;
  }
  if ((Y > yUpper) && (Y < yLower)) { // y out of range
    check = true;
  }
  if ((Z > zUpper) && (Z < zLower)) { // z out of range
    check = true;
  }
}

// This function records gesture and checks entered gesture
// Pass true to this function to record a new gesture
// Pass false to this function to enter or check a gesture
void gesture(bool op) {
  // retrieve x values
  digitalWrite(8,LOW);
  SPI.transfer(0b10101001);
  uint8_t X = SPI.transfer(0xFF) & 0b01111111;
  digitalWrite(8,HIGH);

  delay(1);

  // retrieve y values
  digitalWrite(8,LOW);
  SPI.transfer(0b10101011);
  uint8_t Y = SPI.transfer(0xFF) & 0b01111111;
  digitalWrite(8,HIGH);

  delay(1);

  // retrieve z values
  digitalWrite(8,LOW);
  SPI.transfer(0b10101101);
  uint8_t Z = SPI.transfer(0xFF) & 0b01111111;
  digitalWrite(8,HIGH);

  // Print retrieved x, y, x values
  Serial.print("X = ");
  Serial.print(X);
  Serial.print("\tY = ");
  Serial.print(Y);
  Serial.print("\tZ = ");
  Serial.println(Z);

  delay(50);

  if (op == true) { // if user is recording gesture
    x[recordIndex] = X;
    y[recordIndex] = Y;
    z[recordIndex] = Z;
    recordIndex = recordIndex + 1; // increment the record index
  } else { // if user is entering or checking gesture
    // compute x tolerance range 
    if (x[enterIndex] > (127-toleranceRange)) {
      wraparound = true;
      xUpper = toleranceRange - (127-x[enterIndex]);
      xLower = x[enterIndex] - toleranceRange;
    } else if (x[enterIndex] < toleranceRange) {
      wraparound = true;
      xUpper = x[enterIndex] + toleranceRange;
      xLower = 127 - (toleranceRange - x[enterIndex]);
    } else {
      xUpper = x[enterIndex] + toleranceRange;
      xLower = x[enterIndex] - toleranceRange;
    }
    // compute y tolerance range
    if (y[enterIndex] > (127-toleranceRange)) {
      wraparound = true;
      yUpper = toleranceRange - (127-y[enterIndex]);
      yLower = y[enterIndex] - toleranceRange;
    } else if (y[enterIndex] < toleranceRange) {
      wraparound = true;
      yUpper = y[enterIndex] + toleranceRange;
      yLower = 127 - (toleranceRange - y[enterIndex]);
    } else {
      yUpper = y[enterIndex] + toleranceRange;
      yLower = y[enterIndex] - toleranceRange;
    }
    // compute z tolerance range
    if (z[enterIndex] > (127-toleranceRange)) {
      wraparound = true;
      zUpper = toleranceRange - (127-z[enterIndex]);
      zLower = z[enterIndex] - toleranceRange;
    } else if (z[enterIndex] < toleranceRange) {
      wraparound = true;
      zUpper = z[enterIndex] + toleranceRange;
      zLower = 127 - (toleranceRange - z[enterIndex]);
    } else {
      zUpper = z[enterIndex] + toleranceRange;
      zLower = z[enterIndex] - toleranceRange;
    }
    if (wraparound) { // if wraparound range is used, call checkWraparoundValues() to check the values
      checkWraparoundValue(X, Y, Z); 
      wraparound = false;
    } else { // if normal range is used, call checkValue() to check the values
      checkValue(X, Y, Z);
    }
    if(check) { // if value is out of range, count plus one
      count = count + 1; 
      check = false;
    }
    if (count > toleranceCount) { // if count is greater than tolerable count, the end check and output incorrect gesture
      entering = false;
      Serial.println("End check....");
      CircuitPlayground.playTone(100,1000); // play incorrect sound on the speaker
      Serial.println("Incorrect gesture");
      strip.fill(red, 0, 10); // light up the neo pixel with red
      strip.show(); // push the color to the neo pixels
    } else if (enterIndex == recordIndex) { // if count is less than tolerable count and all values are checked, output correct gesture
      entering = false;
      Serial.println("End check....");
      CircuitPlayground.playTone(1700,100); // play correct sound on the speaker
      delay(10);
      CircuitPlayground.playTone(1700,100);
      delay(10);
      CircuitPlayground.playTone(1700,100);
      Serial.println("Passed, correct gesture");
      strip.fill(green, 0, 10); // light up the neo pixel with green 
      strip.show(); // push the color to the neo pixels
    }
    enterIndex = enterIndex + 1; // increment the enter index
  }
}

void setup() {
  SPISettings MySettings(1000000, MSBFIRST, SPI_MODE3);
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);

  //Lets set up the SPI bus
  SPI.begin();
  SPI.beginTransaction(MySettings);

  Serial.begin(9600);
  digitalWrite(8,LOW);
  SPI.transfer(0b00100000);
  SPI.transfer(0b01010111);
  digitalWrite(8,HIGH);

  // Buttons and LED initialization 
  pinMode(recordPin, INPUT); // Right button as record key
  pinMode(enterPin, INPUT); // Left button as enter key
  pinMode(13, OUTPUT); // On board LED
  pinMode(31, OUTPUT); // On board speaker

  // Neo Pixels setup
  strip.begin();
  strip.setBrightness(30);
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
}

void loop() {
  recordButton = digitalRead(recordPin); // read record button state

  // 
  if (recording == false) {
    digitalWrite(LEDPin, LOW);
    if (recordButton == HIGH) {
      recordIndex = 0;
      recording = true;
      strip.clear();
      strip.show();
      Serial.println("Start Recording.....");
      delay(200);
    }
  } 

  recordButton = digitalRead(recordPin); // read record button state

  if (recording == true) {
    digitalWrite(LEDPin, HIGH);
    gesture(true); // pass true to gesture() to record
    if (recordButton == HIGH) {
      recording = false;
      Serial.println("Recording Stop");
      delay(200);
    }
    if (recordIndex == 150) {
      recording = false;
      Serial.println("Reached max recording time");
    }
  }

  enterButton = digitalRead(enterPin); // read enter button state

  if (entering == false) {
    if (enterButton == HIGH) {
      count = 0;
      enterIndex = 0;
      entering = true;
      toleranceCount = recordIndex / 3.2;
      strip.clear();
      strip.show();
      delay(200);
      Serial.println("Enter gesture....");
    }
  } 

  enterButton = digitalRead(enterPin); // read enter button state

  if (entering == true) {
    gesture(false); // pass false to gesture() to enter/check
    if (enterButton == HIGH) {
      entering = false;
      Serial.println("End check");
      delay(200);
    }
  }

  delay(50);
}

