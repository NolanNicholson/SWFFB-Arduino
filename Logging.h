// Logging code cribbed from Necroware's GamePort Adapter firmware.
// Copyright (C) 2021 Necroware

#pragma once

/// Debug messages on serial port are turned off by default. Comment the following
/// line to enable logging to the serial port.
/// Arduino Micro seems somehow to share the serial port with the USB interface.
/// If the serial port will be activated, the operating system will no longer
/// recognize the USB device!
//#define NDEBUG

#ifdef NDEBUG
#define initLog()
#define log(...)
#else
inline void initLog() {
    Serial.begin(9600);
    while(!Serial); 
}

inline void log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char buffer[512];
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  Serial.println(buffer);
}
#endif // !NDEBUG