#include "pitches.h"
#define BUZZER  12
#include <NewTone.h>

// notes in the melody:
int melodyOpen[] = {
  0, NOTE_C4, NOTE_D4, NOTE_E4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsOpen[] = {
  4, 4, 4, 4
};

void beepOpen() {
  noInterrupts();
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsOpen[thisNote];
    NewTone(BUZZER, melodyOpen[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noNewTone(BUZZER);
    interrupts();
  }
}

// notes in the melody:
int melodyWarning[] = {
  0, NOTE_E4, NOTE_D4, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationsWarning[] = {
  4, 4, 4, 4
};

void beepWarning() {
  noInterrupts();
  for (int thisNote = 0; thisNote < 4; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsWarning[thisNote];
    NewTone(BUZZER, melodyWarning[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noNewTone(BUZZER);
    interrupts();
  }
}
