#pragma once

#include <ArduinoJson.h>
#include "mbedtls/base64.h"
#include "esp_heap_caps.h"
#include "LvglMain.h"
#include "JsonSerial.h"
#include "Validation.h"
#include "Led.h"
#include "LvglMain.h"
#include "SpiRamAllocator.h"
#include "SerialMessages.h"
#include "preferences/Wallpaper.h"

static const char *TAG_SERIAL_HANDLER = "SERIAL_HANDLER";

static JsonSerial serial(TINYUSB_CDC_ACM_0);

static void sendSetWallpaperCompletedMessage()
{
  JsonDocument doc(&spiRamAllocator);
  doc["type"] = MessageTypes::SET_WALLPAPER_COMPLETED;
  serial.send(doc);
}

static void sendGetDataResponseMessage()
{
  LedData *ledData = loadLedData();
  DisplayData *displayData = loadDisplayData();

  JsonDocument doc(&spiRamAllocator);
  doc["type"] = MessageTypes::GET_DATA_RESPONSE;
  auto led = doc["led"].to<JsonObject>();
  led["color"] = ledData->color;
  led["brightness"] = ledData->brightness;
  auto display = doc["display"].to<JsonObject>();
  display["accentColor"] = displayData->accent_color;
  display["brightness"] = displayData->brightness;
  serial.send(doc);
}

static void onJson(JsonDocument &doc)
{
  ESP_LOGI(TAG_SERIAL_HANDLER, "RECIEVED JSON");
  if (!doc["type"].is<const char *>())
  {
    return;
  }

  const char *type = doc["type"].as<const char *>();

  if (strcmp(type, MessageTypes::SET_LED) == 0)
  {
    if (
        !doc["color"].is<const char *>() ||
        !doc["brightness"].is<uint8_t>())
    {
      return;
    }

    const char *color = doc["color"].as<const char *>();
    uint8_t brightness = doc["brightness"].as<uint8_t>();

    if (!isValidHexColor(color))
    {
      return;
    }

    LedData ledData = {};
    ledData.brightness = brightness;
    strncpy(ledData.color, color, 8);
    saveLedData(&ledData);
    set_rgb_hex(color, brightness);
  }
  else if (strcmp(type, MessageTypes::SET_DISPLAY) == 0)
  {
    if (
        !doc["accentColor"].is<const char *>() ||
        !doc["brightness"].is<uint8_t>())
    {
      return;
    }

    const char *accentColor = doc["accentColor"].as<const char *>();
    uint8_t brightness = doc["brightness"].as<uint8_t>();

    if (!isValidHexColor(accentColor))
    {
      return;
    }

    DisplayData displayData = {};
    displayData.brightness = brightness;
    strncpy(displayData.accent_color, accentColor, 8);
    saveDisplayData(&displayData);
    set_tft_brightness(brightness);

    LvglMessage lvglMessage{};
    lvglMessage.type = LvglMessageType::SetAccentColor;
    strncpy(lvglMessage.data.setAccentColorLvglMessage.accent_color, accentColor, 8);
    xQueueSend(lvgl_message_queue_handle, &lvglMessage, 0);
  }
  else if (strcmp(type, MessageTypes::PC_STATS) == 0)
  {
    if (
        !doc["cpu"].is<uint8_t>() ||
        !doc["ram"].is<uint8_t>())
    {
      return;
    }

    uint8_t cpu = doc["cpu"].as<uint8_t>();
    uint8_t ram = doc["ram"].as<uint8_t>();

    LvglMessage lvglMessage{};
    lvglMessage.type = LvglMessageType::SetStats;
    lvglMessage.data.setStatsLvglMessage.cpu = cpu;
    lvglMessage.data.setStatsLvglMessage.ram = ram;
    xQueueSend(lvgl_message_queue_handle, &lvglMessage, 0);
  }
  else if (strcmp(type, MessageTypes::GET_DATA) == 0)
  {
    sendGetDataResponseMessage();
  }
  else
  {
    return;
  }

  char str[1024];
  serializeJson(doc, str, sizeof(str));
  ESP_LOGI(TAG_SERIAL_HANDLER, "%s", str);
}

static bool decodeImageBase64(
    const char *base64,
    size_t base64_length)
{
  if (base64_length != 204800)
  {
    return false;
  }

  if (!wallpaperDataCache)
  {
    return false;
  }

  size_t decoded_size = 0;

  int result = mbedtls_base64_decode(
      wallpaperDataCache->image,
      IMAGE_SIZE,
      &decoded_size,
      reinterpret_cast<const unsigned char *>(base64),
      base64_length);

  if (result != 0 || decoded_size != IMAGE_SIZE)
  {
    return false;
  }

  return true;
}

static void onImage(const char *type, size_t typeLen,
                    const char *base64, size_t base64Len)
{
  if (!decodeImageBase64(base64, base64Len))
  {
    ESP_LOGE(TAG_SERIAL_HANDLER, "image decode failed");
    return;
  }

  saveWallpaperData();
  sendSetWallpaperCompletedMessage();

  LvglMessage lvglMessage{};
  lvglMessage.type = LvglMessageType::UpdateWallpaper;
  xQueueSend(lvgl_message_queue_handle, &lvglMessage, 0);

  ESP_LOGI(TAG_SERIAL_HANDLER, "image displayed");
}

static void serial_handler_task(void *arg)
{
  ESP_LOGI(
      TAG_SERIAL_HANDLER,
      "Starting USB CDC serial");

  ESP_ERROR_CHECK(
      serial.begin());

  serial.onJson(onJson);
  serial.onImage(onImage);

  ESP_LOGI(
      TAG_SERIAL_HANDLER,
      "USB CDC serial initialized");

  while (true)
  {
    serial.process();

    vTaskDelay(
        pdMS_TO_TICKS(10));
  }
}