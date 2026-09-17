#pragma once

#include <stdint.h>
#include <cstring>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "Properties.h"
#include "NvsStorage.h"

struct LedData
{
  char color[8]; // "#ffffff" + '\0'
  uint8_t brightness;
};

static LedData *ledDataCache = nullptr;

static void saveLedData(const LedData *data)
{
  if (!data || !preferences)
    return;

  esp_err_t err = nvs_set_blob(
      preferences,
      "led_data",
      data,
      sizeof(LedData));

  if (err != ESP_OK)
    return;

  err = nvs_commit(preferences);

  if (err != ESP_OK)
    return;

  if (!ledDataCache)
  {
    ledDataCache = static_cast<LedData *>(
        heap_caps_malloc(
            sizeof(LedData),
            MALLOC_CAP_SPIRAM));
  }

  if (ledDataCache)
    memcpy(
        ledDataCache,
        data,
        sizeof(LedData));
}

static LedData *loadLedData()
{
  if (ledDataCache)
    return ledDataCache;

  ledDataCache = static_cast<LedData *>(
      heap_caps_malloc(
          sizeof(LedData),
          MALLOC_CAP_SPIRAM));

  if (!ledDataCache)
    return nullptr;

  // Defaults
  memcpy(
      ledDataCache->color,
      "#ffffff",
      sizeof(ledDataCache->color));

  ledDataCache->brightness = 128;

  size_t requiredSize = sizeof(LedData);

  esp_err_t err = nvs_get_blob(
      preferences,
      "led_data",
      ledDataCache,
      &requiredSize);

  if (err != ESP_OK || requiredSize != sizeof(LedData))
  {
    // Keep defaults
    memcpy(
        ledDataCache->color,
        "#ffffff",
        sizeof(ledDataCache->color));

    ledDataCache->brightness = 128;
  }

  return ledDataCache;
}