#pragma once

#include <Zigbee.h>
#include "ledDriver.hpp"

/* Zigbee dimmable light configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10
#define SWITCH_ENDPOINT 11
ZigbeeDimmableLight zbLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeLight zbSparklingSwitch(SWITCH_ENDPOINT);
bool isSparklingModeOn = false;

/********************* Sparkling Switch functions **************************/

void setSparklingMode(bool in_isOn)
{
  isSparklingModeOn = in_isOn;

  if (isSparklingModeOn)
  {
    zbLight.setLightState(false); // disable light (turn off)
    ledDriver.enabledSparkling();
  }
  else
  {
    ledDriver.disabledSparkling();
  }
}

/********************* LED functions **************************/

void setLight(bool in_state, uint8_t in_level)
{
  if (in_state)
  {
    ledDriver.disabledSparkling();
    zbSparklingSwitch.setLight(false); // disabled sparkling
    ledDriver.setAll(in_level);
  }
  else
  {
    ledDriver.setAll(0);
  }
}

// Create a task on identify call to handle the identify function
void identify(uint16_t time)
{
  static bool blink = true;
  ledDriver.disabledSparkling();
  zbSparklingSwitch.setLight(false); // disabled sparkling

  if (time == 0)
  {
    // If identify time is 0, stop blinking and restore light as it was used for identify
    zbLight.restoreLight();
    return;
  }

  ledDriver.setAll(blink ? 255 : 0);
  blink = not blink;
}

void setupZBLight()
{
  zbLight.onLightChange(setLight);
  zbLight.onIdentify(identify);
  zbLight.setManufacturerAndModel("Halvoe", "EnterpriseLight");
  zbSparklingSwitch.setManufacturerAndModel("Halvoe", "SparklingSwitch");
  zbSparklingSwitch.onLightChange(setSparklingMode);
  // Add endpoints to Zigbee Core
  Zigbee.addEndpoint(&zbLight);
  Zigbee.addEndpoint(&zbSparklingSwitch);
}
