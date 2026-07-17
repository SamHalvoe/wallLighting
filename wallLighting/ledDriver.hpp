#pragma once

#include <array>
#include <optional>
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
    const uint8_t m_maxSparkleCount = 32;
    uint8_t m_sparkleCount = 0;
    const uint16_t m_sparklingChance = 250;
    const unsigned long m_sparklingFrameTime = 15; // ms
    elapsedMillis m_timeSinceSparklingFrameUpdate;
    const uint8_t m_sparklingFadeUpSpeed = 6;
    const uint8_t m_sparklingFadeDownSpeed = 3;
    const uint8_t m_offBrightness = 2;
    bool m_isSparklingEnabled = false;
    bool m_wasSparklingStateRendered = false;
    std::optional<uint8_t> m_brightnessPromise;

  private:
    void updateSparklingState()
    {
      if (m_sparkleCount < m_maxSparkleCount && random(1000) < m_sparklingChance)
      {
        size_t ledIndex = random(m_ledCount);

        if (m_ledSparklingStateList[ledIndex] == LEDSparklingState::off)
        {
          ++m_sparkleCount;
          m_ledSparklingStateList[ledIndex] = LEDSparklingState::fadingUp;
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
          if (m_ledSparklingBrightnessList[ledIndex] - m_sparklingFadeDownSpeed <= m_offBrightness)
          {
            --m_sparkleCount;
            m_ledSparklingBrightnessList[ledIndex] = m_offBrightness;
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
          ledDriver.analogWrite(index, m_ledSparklingBrightnessList[index]);
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

    void setAllAsync(uint8_t in_brightness)
    {
      m_brightnessPromise = in_brightness;
    }

    void enabledSparkling()
    {
      if (not m_isSparklingEnabled)
      {
        m_ledSparklingStateList.fill(LEDSparklingState::off);
        m_ledSparklingBrightnessList.fill(m_offBrightness);
        m_sparkleCount = 0;
        m_isSparklingEnabled = true;
      }
    }

    void disabledSparkling()
    {
      m_isSparklingEnabled = false;
    }

    void run()
    {
      if (m_isSparklingEnabled && m_timeSinceSparklingFrameUpdate >= m_sparklingFrameTime)
      {
        updateSparklingState();
        renderSparklingState();
        m_timeSinceSparklingFrameUpdate = 0;
        m_wasSparklingStateRendered = true;
      }
      else if (m_brightnessPromise.has_value())
      {
        setAll(m_brightnessPromise.value());
        m_brightnessPromise.reset();
        m_wasSparklingStateRendered = false;
      }
      else if (not m_isSparklingEnabled && m_wasSparklingStateRendered)
      {
        setAll(0);
        m_wasSparklingStateRendered = false;
      }
    }
};

LEDDriver ledDriver;
