#pragma once

#include "Utilities.h"
#include "DigitalPin.h"

DigitalOutput<A8> m_triggerX;
DigitalOutput<A9> m_triggerY;

#define FFB_EFFECT_CONSTANT 0x12
#define FFB_EFFECT_SINE     0x02
#define FFB_EFFECT_SQUARE   0x05
#define FFB_EFFECT_RAMP     0x06
#define FFB_EFFECT_TRIANGLE 0x08
#define FFB_EFFECT_SPRINT   0x0D
#define FFB_EFFECT_INERTIA  0x0F
#define FFB_EFFECT_FRICTION 0x10

void WaitMs(int ms)
{
  while(ms--)
  {
    //delayMicroseconds(1000); // never works
    _delay_ms(1); // works sometimes; requires a compile-time integer constant
  }
}

void cooldown()
{
  m_triggerX.setLow();
  m_triggerY.setLow();

  delayMicroseconds(7000);
}

void SidewinderFFBProInitPulses(int count)
{
  while (count--)
  {
    m_triggerX.setHigh();
    m_triggerY.setHigh();

    delayMicroseconds(50);

    m_triggerX.setLow();
    m_triggerY.setLow();

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
    delay(69);
    Serial1.write(ac_disable, sizeof(ac_disable));
  }
}

void FFBInitEnableFFBMode()
{
  // Magic numbers for initial "handshake"
  const int handshake_delays[] = { 100,   7,  35,  14,  78,   4,  59 }; // works sometimes (when interrupted?)
  const int handshake_pulses[] = {   1,   4,   3,   2,   2,   3,   2 };

  // Transmit "handshake", to prepare the joystick to receive MIDI data.
  //log("FFB init: Performing pre-MIDI handshake...");

  cooldown();

  for (int i = 0; i < 7; i++)
  {
    delay(handshake_delays[i]); // in ms; works occasionally
    //WaitMs(handshake_delays[i]);
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
  delayMicroseconds(20000);

  Serial1.write(startupFfbData_1, sizeof(startupFfbData_1));	// Init
  delayMicroseconds(56000);

  Serial1.write(startupFfbData_2, sizeof(startupFfbData_2));	// Initialize effects data memory
}

void FFBConstantRight()
{

  const uint8_t data[] = {
    0xf0, // 0: SysEx start - effect data
    0x00, 0x01, 0x0a, 0x01, 0x23, // 1..5: Effect header
    0x12, // 6: Waveform type (0x12 is Constant)
    0x7f, // 7: not used?
    0x5a, 0x19, // 8, 9: Duration (LSB, MSB; number in 2ms)
    0x00, 0x00, // 10, 11: not used?
    0x0e, 0x02, // 12, 13: Direction (B1, B2; degrees; e.g. 0x0e02 for right, 0x0000 for down)
    0x7f, 0x64, 0x00, 0x10, 0x4e, // 14..18: not used?
    0x7f, // 19: Envelope Attack
    0x00, 0x00, // 20, 21: Envelope Attack Time
    0x7f, 0x5a, // 22, 23: Envelope Fade Time
    0x19, // 24: not used?
    0x7f, // 25: Envelope Fade
    0x01, // 26: Wavelength [0x6f..0x01 => 1/value Hz]
    0x00, // 27: not used?
    0x7f, 0x00, // 28, 29: constant dir?
    0x00, 0x00, // 30, 31: not used?
    0x18, // 32: Checksum
    0xf7, // SysEx end - effect data

    0xb5, 0x20, 0x02 // play
  };

  Serial1.write(data, sizeof(data));
  delayMicroseconds(20000);

}

void FFBSine()
{
  const uint8_t data[] = {
    0xf0, // 0: SysEx start - effect data
    0x00, 0x01, 0x0a, 0x01, 0x23,
    0x02,
    0x7f,
    0x09, 0x16,
    0x00, 0x00,
    0x00, 0x00,
    0x7f, 0x64, 0x00, 0x10, 0x4e,
    0x7f,
    0x00, 0x00,
    0x7f, 0x09,
    0x16, 0x7f, 0x01, 0x00,
    0x7f, 0x00,
    0x01, 0x01,
    0x5e,
    0xf7, // SysEx end - effect data

    0xb5, 0x20, 0x02 // play
  };

  Serial1.write(data, sizeof(data));
  delayMicroseconds(20000);
}

void FFBSendEffect()
{
  uint8_t data[] = {
    0xf0, // 0: SysEx start - effect data
    0x00, 0x01, 0x0a, 0x01, // 1..4: SysEx header
    0x23, // 5: 0x23 = this is effect data!
    0x12, // 6: Waveform type (0x12 is Constant)
    0x7f, // 7: not used?
    0x5a, 0x19, // 8, 9: Duration (LSB, MSB; number in 2ms)
    0x00, 0x00, // 10, 11: not used?
    0x00, 0x00, // 12, 13: Direction (B1, B2; degrees; e.g. 0x0e02 for right, 0x0000 for down)
    0x7f, 0x64, 0x00, 0x10, 0x4e, // 14..18: not used?
    0x7f, // 19: Envelope Attack
    0x00, 0x00, // 20, 21: Envelope Attack Time
    0x7f, 0x5a, // 22, 23: Envelope Fade Time
    0x19, // 24: not used?
    0x7f, // 25: Envelope Fade
    0x01, // 26: Wavelength [0x6f..0x01 => 1/value Hz]
    0x00, // 27: not used?
    0x7f, 0x00, // 28, 29: constant dir?
    0x00, 0x00, // 30, 31: not used?
    0x00, // 32: Checksum
    0xf7, // SysEx end - effect data

    0xb5, 0x20, 0x02 // play
  };

  // u14: Value = Byte1 + Byte2 x 128 (i.e. Byte1 | Byte2 <<7) Max value of each byte is 0x7f
  // u14i: like u14i, but 0x0000 = infinite

  // Waveform type
  data[6] = FFB_EFFECT_SINE;

  // Duration [u14i (0 = inf), increments of 2ms]
  uint16_t duration = 500;
  data[8] = duration & 0x7f;
  data[9] = (duration >> 7 & 0x7f);

  // Direction [u14, deg]
  uint16_t direction = 0;
  data[12] = direction & 0x7f;
  data[13] = (direction >> 7) & 0x7f;

  // Envelope
  uint8_t envelope_attack = 400;
  uint8_t envelope_fade = 0;
  uint16_t envelope_attack_time = 100;
  uint16_t envelope_fade_time = 50;

  data[19] = envelope_attack & 0x7f;
  data[20] = envelope_attack_time & 0x7f;
  data[21] = (envelope_attack_time >> 7) & 0x7f;
  data[22] = envelope_fade_time & 0x7f;
  data[23] = (envelope_fade_time >> 7) & 0x7f;
  data[25] = envelope_fade & 0x7f;

  // Frequency [Hz; 0x6f .. 0x01]
  uint8_t frequency = 0x10;
  data[26] = frequency;

  // Produce checksum
  uint8_t checksum = 0;
  for (int i = 5; i < 32; i++)
  {
    checksum += data[i];
  }
  checksum = (0x80 - checksum) & 0x7f;
  data[32] = checksum;

  Serial1.write(data, sizeof(data));
  delayMicroseconds(20000);
}

void FFBInit()
{
  while (true)
  {
    FFBInitEnableFFBMode();

    FFBInitStartupMidi();

    // Disable auto-center effect
    SidewinderFFBProSetAutoCenter(false);

    delay(100);

    FFBSendEffect();
    //FFBSine();

    delay(2000);
  }
}