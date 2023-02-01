#pragma once

#include "Logging.h"

#define PIN_X1 A8

// low-level stuff from adapt-ffb-joy
#define TRGDDR DDRB
#define TRGX1BIT DDB4
#define TRGY2BIT DDB5
#define set_bit( sfr, bit )	(_SFR_BYTE(sfr) |=  _BV(bit))
#define clr_bit( sfr, bit )	(_SFR_BYTE(sfr) &= ~_BV(bit))

void cooldown()
{
  set_bit(TRGDDR, TRGX1BIT);
  set_bit(TRGDDR, TRGY2BIT);
  delayMicroseconds(1000);
}

void SidewinderFFBProInitPulses(int count)
{
  while (count--)
  {
    //X1_pull();
    do
    {
      clr_bit(TRGDDR, TRGX1BIT);
      clr_bit(TRGDDR, TRGY2BIT);
    } while (0);

    //digitalWrite(PIN_X1, HIGH);
    delayMicroseconds(50);

    //X1_rel();
    do
    {
      set_bit(TRGDDR, TRGX1BIT);
      set_bit(TRGDDR, TRGY2BIT);
    } while (0);

    //digitalWrite(PIN_X1, LOW);
    delayMicroseconds(170);
  }
}

void SidewinderFFBProSetAutoCenter(bool enable)
{
  const uint8_t ac_enable[] = {
    0xc5, 0x01
  };

  const uint8_t ac_disable[] = {
    0xb5, 0x7c, 0x7f,
    0xa5, 0x7f, 0x00,
    0xc5, 0x06,
  };

  Serial1.write(ac_enable, sizeof(ac_enable));
  if (!enable) {
    delay(70);
    Serial1.write(ac_disable, sizeof(ac_disable));
  }
}

void FFBInitEnableFFBMode()
{
  // Magic numbers for initial "handshake"
  const int handshake_delays[] = { 100,   7,  35,  14,  78,   4,  59 };
  const int handshake_pulses[] = {   1,   4,   3,   2,   2,   3,   2 };

  // Transmit "handshake", to prepare the joystick to receive MIDI data.
  //log("FFB init: Performing pre-MIDI handshake...");

  cooldown();

  for (int i = 0; i < 7; i++)
  {
    delay(handshake_delays[i]);
    SidewinderFFBProInitPulses(handshake_pulses[i]);
  }
}

void FFBInitStartupMidi()
{
  const uint8_t startupFfbData_0[] = {
    0xc5, 0x01        // <ProgramChange> 0x01
  };

  const uint8_t startupFfbData_1[] = {
    0xf0,
    0x00, 0x01, 0x0a, 0x01, 0x10, 0x05, 0x6b,  // ???? - reset all?
    0xf7
  };

  const uint8_t startupFfbData_2[] = {
    0xb5, 0x40, 0x7f,  // <ControlChange>(Modify, 0x7f)
    0xa5, 0x72, 0x57,  // offset 0x72 := 0x57
    0xb5, 0x44, 0x7f,
    0xa5, 0x3c, 0x43,
    0xb5, 0x48, 0x7f,
    0xa5, 0x7e, 0x00,
    0xb5, 0x4c, 0x7f,
    0xa5, 0x04, 0x00,
    0xb5, 0x50, 0x7f,
    0xa5, 0x02, 0x00,
    0xb5, 0x54, 0x7f,
    0xa5, 0x02, 0x00,
    0xb5, 0x58, 0x7f,
    0xa5, 0x00, 0x7e,
    0xb5, 0x5c, 0x7f,
    0xa5, 0x3c, 0x00,
    0xb5, 0x60, 0x7f,
    0xa5, 0x14, 0x65,
    0xb5, 0x64, 0x7f,
    0xa5, 0x7e, 0x6b,
    0xb5, 0x68, 0x7f,
    0xa5, 0x36, 0x00,
    0xb5, 0x6c, 0x7f,
    0xa5, 0x28, 0x00,
    0xb5, 0x70, 0x7f,
    0xa5, 0x66, 0x4c,
    0xb5, 0x74, 0x7f,
    0xa5, 0x7e, 0x01,
  };

  // Send MIDI data.

  Serial1.write(startupFfbData_0, sizeof(startupFfbData_0));	// Program change
  delay(20);

  Serial1.write(startupFfbData_1, sizeof(startupFfbData_1));	// Init
  delay(56);

  Serial1.write(startupFfbData_2, sizeof(startupFfbData_2));	// Initialize effects data memory
}

void FFBInit()
{
  FFBInitEnableFFBMode();
  FFBInitStartupMidi();

  // Disable auto-center effect.
  SidewinderFFBProSetAutoCenter(false);
  delay(70);
}