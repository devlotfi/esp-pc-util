#pragma once

#include <ArduinoJson.h>

#include "JsonSerial.h"

static const char *TAG_SERIAL_HANDLER = "SERIAL_HANDLER";

static JsonSerial serial(
    TINYUSB_CDC_ACM_0);

static void onJson(JsonDocument &doc)
{
  char str[1024];
  serializeJson(doc, str, sizeof(str));
  ESP_LOGI(TAG_SERIAL_HANDLER, "%s", str);
}

static void serial_handler_task(void *arg)
{
  ESP_LOGI(
      TAG_SERIAL_HANDLER,
      "Starting USB CDC serial");

  ESP_ERROR_CHECK(
      serial.begin());

  serial.onJson(onJson);

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