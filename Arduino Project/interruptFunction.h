volatile bool show = false;

void intShow() {
  show = true;
}

ISR(PCINT2_vect) { // Port D, PCINT16 - PCINT23
  show = true;
}

ISR(PCINT1_vect) { // // Port C, PCINT8 - PCINT14
  show = true;
}

ISR(PCINT0_vect) { // // Port B, PCINT0 - PCINT7
  show = true;
}
