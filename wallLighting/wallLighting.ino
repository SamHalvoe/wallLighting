#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include <Zigbee.h>
#include <elapsedMillis.h>
#include <Adafruit_AW9523.h>

#include "zbLight.hpp"

uint8_t bootButton = BOOT_PIN;
elapsedSeconds timeSinceZigbeeNotConnected;
const unsigned long ZIGBEE_NOT_CONNECTED_TIMEOUT = 300; // s

Adafruit_AW9523 ledDriver0;
Adafruit_AW9523 ledDriver1;
Adafruit_AW9523 ledDriver2;
Adafruit_AW9523 ledDriver3;
elapsedMillis ledTimer0;
bool ledState0 = false;

void factoryResetIfBootIsPressed()
{
  // Checking button for factory reset
  if (digitalRead(bootButton) == LOW) // Push button pressed
  {
    // Key debounce handling
    delay(100);
    int startTime = millis();

    while (digitalRead(bootButton) == LOW) // Push button pressed
    {
      delay(50);

      if ((millis() - startTime) > 3000)
      {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        rgbLedWrite(ledBuildin, 0, 255, 0); // red
        delay(1000);
        rgbLedWrite(ledBuildin, 0, 0, 0);
        Zigbee.factoryReset();
      }
    }
  }
}

void setupZigbee()
{
  // When all EPs are registered, start Zigbee in End Device mode
  if (not Zigbee.begin(ZIGBEE_END_DEVICE))
  {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting in 1s...");
    rgbLedWrite(ledBuildin, 0, 255, 255); // purple
    delay(1000);
    rgbLedWrite(ledBuildin, 0, 0, 0);
    ESP.restart();
  }

  Serial.println("Connecting to network");

  timeSinceZigbeeNotConnected = 0;
  while (not Zigbee.connected())
  {
    Serial.print(".");
    rgbLedWrite(ledBuildin, 0, 0, 255); // blue
    delay(100);
    rgbLedWrite(ledBuildin, 0, 0, 0);
    delay(100);

    if (timeSinceZigbeeNotConnected >= ZIGBEE_NOT_CONNECTED_TIMEOUT)
    {
      Serial.println("Zigbee failed to connect!");
      Serial.println("Rebooting in 1s...");
      rgbLedWrite(ledBuildin, 255, 255, 0); // yellow or yellowish green
      delay(1000);
      rgbLedWrite(ledBuildin, 0, 0, 0);
      ESP.restart();
    }

    factoryResetIfBootIsPressed();
  }
}

void setup()
{
  // SCL on pin 22 & SDA on pin 12
  ledDriver0.begin(AW9523_DEFAULT_ADDR);
  ledDriver0.configureLEDMode(0xFFFF);
  ledDriver1.begin(AW9523_DEFAULT_ADDR + 1);
  ledDriver1.configureLEDMode(0xFFFF);
  ledDriver2.begin(AW9523_DEFAULT_ADDR + 2);
  ledDriver2.configureLEDMode(0xFFFF);
  ledDriver3.begin(AW9523_DEFAULT_ADDR + 3);
  ledDriver3.configureLEDMode(0xFFFF);

  Serial.begin(9600);
  elapsedMillis timeSinceSerialBegin;
  while (not Serial && timeSinceSerialBegin <= 5000) delay(100);
  rgbLedWrite(ledBuildin, 0, 0, 0);
  
  for (uint8_t index = 0; index < 16; ++index)
  {
    ledDriver0.analogWrite(index, 255);
    ledDriver1.analogWrite(index, 255);
    ledDriver2.analogWrite(index, 255);
    ledDriver3.analogWrite(index, 255);
  }

  // Init RMT and leave light OFF
  rgbLedWrite(ledBuildin, 0, 0, 0);
  // Init button for factory reset
  pinMode(bootButton, INPUT_PULLUP);

  //setupZBLight();
  //setupZigbee();

  rgbLedWrite(ledBuildin, 255, 0, 0); // green
  delay(500);
  rgbLedWrite(ledBuildin, 0, 0, 0);
}

void loop()
{
  //factoryResetIfBootIsPressed();
}
