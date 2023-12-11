/*!
 * @file Adafruit_Circuit_Playground.h
 *
 * This is part of Adafruit's CircuitPlayground driver for the Arduino platform.
 * It is designed specifically to work with the Adafruit CircuitPlayground
 * boards.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Ladyada and others for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_CIRCUITPLAYGROUND_H_
#define _ADAFRUIT_CIRCUITPLAYGROUND_H_

#include "Copied_NeoPixel.h"
#include <Arduino.h>

#ifdef __AVR__                    // Circuit Playground 'classic'

#define CPLAY_NEOPIXELPIN 17      ///< neopixel pin
#endif

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with
   CircuitPlayground hardware
*/
/**************************************************************************/
class Adafruit_CircuitPlayground {
public:
  bool begin(uint8_t brightness = 20);

  Adafruit_CPlay_NeoPixel strip;  ///< the neopixel strip object
  
  /**************************************************************************/
  /*!
    @brief turn off all neopixels on the board
  */
  /**************************************************************************/
  void clearPixels(void) {
    strip.clear();
    strip.show();
  }

  /**************************************************************************/
  /*!
    @brief set the color of a neopixel on the board
    @param p the pixel to set. Pixel 0 is above the pad labeled 'GND' right next
    to the USB connector, while pixel 9 is above the pad labeled '3.3V' on the
    other side of the USB connector.
    @param c a 24bit color value to set the pixel to
  */
  /**************************************************************************/
  void setPixelColor(uint8_t p, uint32_t c) {
    strip.setPixelColor(p, c);
    strip.show();
  }

  /**************************************************************************/
  /*!
    @brief set the color of a neopixel on the board
    @param p the pixel to set. Pixel 0 is above the pad labeled 'GND' right next
    to the USB connector, while pixel 9 is above the pad labeled '3.3V' on the
    other side of the USB connector.
    @param r a 0 to 255 value corresponding to the red component of the desired
    color.
    @param g a 0 to 255 value corresponding to the green component of the
    desired color.
    @param b a 0 to 255 value corresponding to the blue component of the desired
    color.
  */
  /**************************************************************************/
  void setPixelColor(uint8_t p, uint8_t r, uint8_t g, uint8_t b) {
    strip.setPixelColor(p, r, g, b);
    strip.show();
  }

  /*!  @brief set the global brightness of all neopixels.
       @param b a 0 to 255 value corresponding to the desired brightness. The
     default brightness of all neopixels is 30. */
  void setBrightness(uint16_t b) { strip.setBrightness(b); }

  /*!  @brief Get a sinusoidal value from a sine table
       @param x a 0 to 255 value corresponding to an index to the sine table
       @returns An 8-bit sinusoidal value back */
  uint8_t sine8(uint8_t x) { return strip.sine8(x); }

  /*!  @brief Get a gamma-corrected value from a gamma table
       @param x a 0 to 255 value corresponding to an index to the gamma table
       @returns An 8-bit gamma-corrected value back */
  uint8_t gamma8(uint8_t x) { return strip.gamma8(x); }

  uint32_t colorWheel(uint8_t x);

  // Basic RGB color sensing with the light sensor and nearby neopixel.
  // Both functions do the same thing and just differ in how they return the
  // result, either as explicit RGB bytes or a 24-bit RGB color value.
  void senseColor(uint8_t &red, uint8_t &green, uint8_t &blue);

  /**************************************************************************/
  /*!
    @brief detect a color using the onboard light sensor
    @return a 24 bit color. The most significant byte is red, followed by green,
    and the least significant byte is blue.
  */
  /**************************************************************************/
  uint32_t senseColor() {
    // Use the individual color component color sense function and then
    // recombine tbe components into a 24-bit color value.
    uint8_t red, green, blue;
    senseColor(red, green, blue);
    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | blue;
  }

  bool isExpress(void);

private:
};

extern Adafruit_CircuitPlayground
    CircuitPlayground; ///< instantiated by default

#endif


/*!
 * @file Adafruit_CircuitPlayground.cpp
 *
 * @mainpage Adafruit CircuitPlayground Library
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's CircuitPlayground driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit CircuitPlayground boards:
 *  - https://www.adafruit.com/products/3000
 *  - https://www.adafruit.com/products/3333
 *
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * @section author Author
 *
 * Written by Ladyada and others for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

// #include <Adafruit_Circuit_Playground.h>

/**************************************************************************/
/*!
    @brief  Set up the CircuitPlayground hardware
    @param  brightness Optional brightness to set the neopixels to
    @returns True if device is set up, false on any failure
*/
/**************************************************************************/
bool Adafruit_CircuitPlayground::begin(uint8_t brightness) {


  strip = Adafruit_CPlay_NeoPixel(10, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(brightness);

  return true;
}

/**************************************************************************/
/*!
    @brief get a color value from the color wheel.
    @param WheelPos a value 0 to 255
    @returns a color value. The colours are a transition r - g - b - back to r.
*/
/**************************************************************************/
uint32_t Adafruit_CircuitPlayground::colorWheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/**************************************************************************/
/*!
    @brief detect a color value from the light sensor
    @param red the pointer to where the red component should be stored.
    @param green the pointer to where the green component should be stored.
    @param blue the pointer to where the blue component should be stored.
*/
/**************************************************************************/


/**************************************************************************/
/*!
    @brief check whether or not this device is a CircuitPlayground Express.
    @returns True if the device is a CircuitPlayground Express, false if it is a
   'classic'.
*/
/**************************************************************************/
bool Adafruit_CircuitPlayground::isExpress(void) {
#ifdef __AVR__
  return false;
#else
  return true;
#endif
}

// instantiate static
Adafruit_CircuitPlayground CircuitPlayground;
