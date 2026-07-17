#pragma once

#include <Zigbee.h>
#include "ledDriver.hpp"

/* Zigbee light configuration */
const uint8_t ZIGBEE_LIGHT_ENDPOINT = 10;
const uint8_t ZIGBEE_SPARKLING_SWITCH_ENDPOINT = 11;
ZigbeeDimmableLight zbLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeLight zbSparklingSwitch(ZIGBEE_SPARKLING_SWITCH_ENDPOINT);

/********************* Sparkling Switch functions **************************/

void setSparklingMode(bool in_isOn)
{
  if (in_isOn)
  {
    if (zbLight.getLightState())
    {
      zbLight.setLight(false, zbLight.getLightLevel()); // disable light (turn off)
    }

    ledDriver.enabledSparkling();
  }
  else
  {
    ledDriver.disabledSparkling();
  }
}

/********************* Light functions **************************/

void setLight(bool in_state, uint8_t in_level)
{
  if (in_state)
  {
    if (zbSparklingSwitch.getLightState())
    {
      zbSparklingSwitch.setLight(false); // disabled sparkling
    }
    
    ledDriver.setAllAsync(in_level);
  }
  else
  {
    ledDriver.setAllAsync(0);
  }
}

// Create a task on identify call to handle the identify function
void identify(uint16_t time)
{
  static bool blink = true;
  ledDriver.disabledSparkling();

  if (time == 0)
  {
    // If identify time is 0, stop blinking and restore light as it was used for identify
    zbLight.restoreLight();
    zbSparklingSwitch.restoreLight();
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
  zbSparklingSwitch.onLightChange(setSparklingMode);
  zbSparklingSwitch.setManufacturerAndModel("Halvoe", "SparklingSwitch");
  // Add endpoints to Zigbee Core
  Zigbee.addEndpoint(&zbLight);
  Zigbee.addEndpoint(&zbSparklingSwitch);
}
