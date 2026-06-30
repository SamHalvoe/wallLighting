#pragma once

#include <Zigbee.h>
#include "ledDriver.hpp"

/* Zigbee dimmable light configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10
ZigbeeDimmableLight zbLight(ZIGBEE_LIGHT_ENDPOINT);

/********************* RGB LED functions **************************/
void setLight(bool in_state, uint8_t in_level)
{
  if (in_state)
  {
    setAllLEDs(in_level);
  }
  else
  {
    setAllLEDs(0);
  }
}

// Create a task on identify call to handle the identify function
void identify(uint16_t time)
{
  static bool blink = true;

  if (time == 0)
  {
    // If identify time is 0, stop blinking and restore light as it was used for identify
    zbLight.restoreLight();
    return;
  }

  setAllLEDs(blink ? 255 : 0);
  blink = not blink;
}

void setupZBLight()
{
  // Set callback functions for RGB and Temperature modes
  zbLight.onLightChange(setLight);
  // Optional: Set callback function for device identify
  zbLight.onIdentify(identify);
  // Optional: Set Zigbee device name and model
  zbLight.setManufacturerAndModel("Halvoe", "EnterpriseLight");
  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbLight);
}
