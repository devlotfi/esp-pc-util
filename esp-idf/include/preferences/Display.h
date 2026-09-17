#pragma once

#include <stdint.h>
#include <cstring>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "Properties.h"
#include "NvsStorage.h"

struct DisplayData
{
  char accent_color[8];
  uint8_t brightness;
};

static DisplayData *displayDataCache = nullptr;

static void saveDisplayData(const DisplayData *data)
{
  if (!data || !preferences)
    return;

  esp_err_t err = nvs_set_blob(
      preferences,
      "display_data",
      data,
      sizeof(DisplayData));

  if (err != ESP_OK)
    return;

  err = nvs_commit(preferences);

  if (err != ESP_OK)
    return;

  if (!displayDataCache)
  {
    displayDataCache = static_cast<DisplayData *>(
        heap_caps_malloc(
            sizeof(DisplayData),
            MALLOC_CAP_SPIRAM));
  }

  if (displayDataCache)
    memcpy(
        displayDataCache,
        data,
        sizeof(DisplayData));
}

static DisplayData *loadDisplayData()
{
  if (displayDataCache)
    return displayDataCache;

  displayDataCache = static_cast<DisplayData *>(
      heap_caps_malloc(
          sizeof(DisplayData),
          MALLOC_CAP_SPIRAM));

  if (!displayDataCache)
    return nullptr;

  // Defaults
  memcpy(
      displayDataCache->accent_color,
      "#894aef",
      sizeof(displayDataCache->accent_color));
  displayDataCache->brightness = 100;

  size_t requiredSize = sizeof(DisplayData);

  esp_err_t err = nvs_get_blob(
      preferences,
      "display_data",
      displayDataCache,
      &requiredSize);

  if (err != ESP_OK || requiredSize != sizeof(DisplayData))
  {
    // Keep defaults
    memcpy(
        displayDataCache->accent_color,
        "#894aef",
        sizeof(displayDataCache->accent_color));
    displayDataCache->brightness = 100;
  }

  return displayDataCache;
}