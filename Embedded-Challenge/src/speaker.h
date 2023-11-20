#ifdef __has_include
    #if __has_include(<Arduino.h>)
        #include <Arduino.h>
    #endif
#endif


#ifndef SONGS
#define SONGS 0
#include "songs.h"
#endif


void buzz(long frequency, long length) {

    OCR3AL = frequency >> 2;
    TCCR3B &= ~(1 << 2);
    TCCR3B &= ~(1 << 1);
    TCCR3B |= (1 << 0);
    delay(length);
    TCCR3B &= ~(1 << 2);
    TCCR3B &= ~(1 << 1);
    TCCR3B &= ~(1 << 0);
    OCR3AL = 0;
    delay(length * 1.3);
}

void sing(int s) {
  // iterate over the notes of the melody:
  if (s == 2) {
    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {

      int noteDuration = 1000 / underworld_tempo[thisNote];

      buzz(underworld_melody[thisNote], noteDuration);

    }

  } else {

    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {

      int noteDuration = 1000 / tempo[thisNote];

      buzz(melody[thisNote], noteDuration);

    }
  }
}
