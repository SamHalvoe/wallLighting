#pragma once

#include <array>
#include <Adafruit_AW9523_CCS.h>
#include <elapsedMillis.h>

class LEDDriver
{
  private:
    enum class LEDSparklingState
    {
      off = 0,
      fadingUp,
      fadingDown
    };

  private:
    // SCL on pin 22 & SDA on pin 12
    std::array<Adafruit_AW9523, 4> m_ledDriverList;

    static const size_t m_ledCount = 64;
    std::array<LEDSparklingState, m_ledCount> m_ledSparklingStateList;
    std::array<uint8_t, m_ledCount> m_ledSparklingBrightnessList;
    const uint16_t m_sparklingChance = 750;
    const unsigned long m_sparklingFrameTime = 15; // ms
    elapsedMillis m_timeSinceSparklingFrameUpdate;
    const uint16_t m_sparklingFadeUpSpeed = 48;
    const uint16_t m_sparklingFadeDownSpeed = 8;
    bool m_isSparklingEnabled = false;

  private:
    void updateSparklingState()
    {
      if (random(1000) < m_sparklingChance)
      {
        size_t ledIndex = random(m_ledCount);

        if (m_ledSparklingStateList[ledIndex] == LEDSparklingState::off)
        {
          m_ledSparklingStateList[ledIndex] = LEDSparklingState::fadingUp;
          m_ledSparklingBrightnessList[ledIndex] = 0;
        }
      }

      for (size_t ledIndex = 0; ledIndex < m_ledCount; ++ledIndex)
      {
        if (m_ledSparklingStateList[ledIndex] == LEDSparklingState::fadingUp)
        {
          if (m_ledSparklingBrightnessList[ledIndex] + m_sparklingFadeUpSpeed >= 255)
          {
            m_ledSparklingBrightnessList[ledIndex] = 255;
            m_ledSparklingStateList[ledIndex] = LEDSparklingState::fadingDown; // Hit the peak, start fading down
          }
          else
          {
            m_ledSparklingBrightnessList[ledIndex] = m_ledSparklingBrightnessList[ledIndex] + m_sparklingFadeUpSpeed;
          }
        }
        else if (m_ledSparklingStateList[ledIndex] == LEDSparklingState::fadingDown)
        {
          if (m_ledSparklingBrightnessList[ledIndex] - m_sparklingFadeDownSpeed <= 0)
          {
            m_ledSparklingBrightnessList[ledIndex] = 0;
            m_ledSparklingStateList[ledIndex] = LEDSparklingState::off; // Sparkle is off
          }
          else
          {
            m_ledSparklingBrightnessList[ledIndex] = m_ledSparklingBrightnessList[ledIndex] - m_sparklingFadeDownSpeed;
          }
        }
      }
    }

    void renderSparklingState()
    {
      for (uint8_t index = 0; index < 16; ++index)
      {
        for (auto& ledDriver : m_ledDriverList)
        {
          ledDriver.analogWrite(index, m_ledSparklingStateList[index] == LEDSparklingState::off ? 0 : m_ledSparklingBrightnessList[index]);
        }
      }
    }

  public:
    LEDDriver() = default;

    void setup()
    {
      for (size_t index = 0; index < m_ledDriverList.size(); ++index)
      {
        m_ledDriverList[index].begin(AW9523_DEFAULT_ADDR + index);
        m_ledDriverList[index].configureLEDCurrent(3);
        m_ledDriverList[index].configureLEDMode(0xFFFF);
      }
    }

    void setAll(uint8_t in_brightness)
    {
      for (uint8_t index = 0; index < 16; ++index)
      {
        for (auto& ledDriver : m_ledDriverList)
        {
          ledDriver.analogWrite(index, in_brightness);
        }
      }
    }

    void enabledSparkling()
    {
      m_isSparklingEnabled = true;
      m_ledSparklingStateList.fill(LEDSparklingState::off);
      m_ledSparklingBrightnessList.fill(0);
      setAll(0);
    }

    void disabledSparkling()
    {
      m_isSparklingEnabled = false;
    }

    void runSparkling()
    {
      if (m_isSparklingEnabled && m_timeSinceSparklingFrameUpdate >= m_sparklingFrameTime)
      {
        updateSparklingState();
        renderSparklingState();
        m_timeSinceSparklingFrameUpdate = 0;
      }
    }
};

LEDDriver ledDriver;
