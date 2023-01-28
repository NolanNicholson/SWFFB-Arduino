// Sidewinder FFB comms prototype
// Nolan Nicholson, 2023
// Based on adapt-ffb-joy by tloimu,
// with code from gameport-adapter by Necroware

#include "FFB.h"
#include "Logging.h"

void setup()
{
  initLog();  

  // Set up trigger pin as an output
  pinMode(PIN_X1, OUTPUT);

  // Set up Arduino's TX line for MIDI communications.
  // (RX is not used by the Sidewinder.)
  log("Setting Serial1 up for MIDI data");
  Serial1.begin(31250, SERIAL_8N1);
  while(!Serial1);

  FFBInit();
}

void loop() { }