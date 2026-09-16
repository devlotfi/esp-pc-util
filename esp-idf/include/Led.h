#pragma once

#include "led_strip.h"
#include "led_strip_rmt.h"
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include "Properties.h"
#include "preferences/Led.h"

static led_strip_handle_t led_strip;

bool set_rgb_hex(const char *hex, uint8_t brightness)
{
  if (hex == nullptr || hex[0] != '#' || strlen(hex) != 7)
    return false;

  for (int i = 1; i < 7; ++i)
  {
    const char c = hex[i];

    if (!((c >= '0' && c <= '9') ||
          (c >= 'A' && c <= 'F') ||
          (c >= 'a' && c <= 'f')))
    {
      return false;
    }
  }

  const uint32_t color = strtoul(hex + 1, nullptr, 16);

  const uint8_t r = (color >> 16) & 0xFF;
  const uint8_t g = (color >> 8) & 0xFF;
  const uint8_t b = color & 0xFF;

  const uint8_t scaled_r = (r * brightness) / 255;
  const uint8_t scaled_g = (g * brightness) / 255;
  const uint8_t scaled_b = (b * brightness) / 255;

  ESP_ERROR_CHECK(
      led_strip_set_pixel(led_strip, 0, scaled_r, scaled_g, scaled_b));

  ESP_ERROR_CHECK(led_strip_refresh(led_strip));

  return true;
}

void setup_led()
{
  led_strip_config_t strip_config = {};
  strip_config.strip_gpio_num = LED_PIN;
  strip_config.max_leds = LED_COUNT;

  led_strip_rmt_config_t rmt_config = {};
  rmt_config.resolution_hz = 10 * 1000 * 1000;
  rmt_config.flags.with_dma = false;

  ESP_ERROR_CHECK(
      led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

  led_strip_clear(led_strip);
  led_strip_refresh(led_strip);

  LedData *ledData = loadLedData();
  ESP_LOGI("led", "%s, %d", ledData->color, ledData->brightness);
  set_rgb_hex(ledData->color, ledData->brightness);
}
