#pragma once

#include <Adafruit_AW9523_CCS.h>

// SCL on pin 22 & SDA on pin 12
Adafruit_AW9523 ledDriver0;
Adafruit_AW9523 ledDriver1;
Adafruit_AW9523 ledDriver2;
Adafruit_AW9523 ledDriver3;

void setupLEDDriver()
{
  ledDriver0.begin(AW9523_DEFAULT_ADDR);
  ledDriver0.configureLEDCurrent(3);
  ledDriver0.configureLEDMode(0xFFFF);
  ledDriver1.begin(AW9523_DEFAULT_ADDR + 1);
  ledDriver1.configureLEDCurrent(3);
  ledDriver1.configureLEDMode(0xFFFF);
  ledDriver2.begin(AW9523_DEFAULT_ADDR + 2);
  ledDriver2.configureLEDCurrent(3);
  ledDriver2.configureLEDMode(0xFFFF);
  ledDriver3.begin(AW9523_DEFAULT_ADDR + 3);
  ledDriver3.configureLEDCurrent(3);
  ledDriver3.configureLEDMode(0xFFFF);
}

void setAllLEDs(uint8_t in_brightness)
{
  Serial.print("setAllLEDs to ");
  Serial.println(in_brightness);
  for (uint8_t index = 0; index < 16; ++index)
  {
    ledDriver0.analogWrite(index, in_brightness);
    ledDriver1.analogWrite(index, in_brightness);
    ledDriver2.analogWrite(index, in_brightness);
    ledDriver3.analogWrite(index, in_brightness);
  }
}