#ifdef __has_include
    #if __has_include(<Arduino.h>)
        #include <Arduino.h>
    #endif
#endif


#ifndef SONGS
#define SONGS 0
#include "songs.h"
#endif


void buzz(uint16_t frequency, long length) {
  PORTC |= (1 << 7);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    PORTC |= (1 << 6);
    delayMicroseconds(delayValue); // wait for the calculated delay value
    PORTC &= ~(1 << 6);
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  PORTC &= ~(1 << 7);

}

void sing(Song s) {
  // iterate over the notes of the melody:
  
  const uint16_t* curr_melody = melodies[s];
  const uint16_t* curr_tempo = tempos[s];

  int size = sizes[s];
  for (int thisNote = 0; thisNote < size; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / pgm_read_word(curr_tempo + thisNote);

    buzz(pgm_read_word(curr_melody + thisNote), noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.20;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    buzz(0, noteDuration);
  }
}