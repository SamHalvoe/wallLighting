#pragma once

#include <array>
#include <Adafruit_AW9523_CCS.h>

// SCL on pin 22 & SDA on pin 12
std::array<Adafruit_AW9523, 4> ledDriverList;

void setupLEDDrivers()
{
  for (size_t index = 0; index < ledDriverList.size(); ++index)
  {
    ledDriverList[index].begin(AW9523_DEFAULT_ADDR + index);
    ledDriverList[index].configureLEDCurrent(3);
    ledDriverList[index].configureLEDMode(0xFFFF);
  }
}

void setAllLEDs(uint8_t in_brightness)
{
  Serial.print("setAllLEDs to ");
  Serial.println(in_brightness);
  for (uint8_t index = 0; index < 16; ++index)
  {
    for (auto& ledDriver : ledDriverList)
    {
      ledDriver.analogWrite(index, in_brightness);
    }
  }
}