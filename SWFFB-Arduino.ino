// Sidewinder FFB comms prototype
// Nolan Nicholson, 2023
// Based on adapt-ffb-joy by tloimu,
// with code from gameport-adapter by Necroware

#include "FFB.h"
#include "Logging.h"

void setup()
{
  initLog();

  set_bit(TRGDDR, TRGX1BIT);
  set_bit(TRGDDR, TRGY2BIT);

  // Set up Arduino's TX line for MIDI communications.
  // (RX is not used by the Sidewinder.)
  Serial1.begin(31250, SERIAL_8N1);
  while(!Serial1);

  delay(2000);

  FFBInit();
}

void loop() { }