#pragma once

#include <ArduinoJson.h>

#include "JsonSerial.h"

static const char *TAG_SERIAL_HANDLER = "SERIAL_HANDLER";

static JsonSerial serial(
    TINYUSB_CDC_ACM_0);

static void onJson(JsonDocument &doc)
{
  const char *type = doc["type"];

  if (type == nullptr)
    return;

  if (strcmp(type, "config") == 0)
  {
    int brightness =
        doc["brightness"] | 0;

    printf(
        "Brightness: %d\n",
        brightness);
  }

  if (strcmp(type, "command") == 0)
  {
    const char *command =
        doc["command"];

    if (command)
    {
      printf(
          "Command: %s\n",
          command);
    }
  }
}

static void serial_handler_task(void *arg)
{
  ESP_LOGI(
      TAG_LVGL_UI,
      "Starting USB CDC serial");

  ESP_ERROR_CHECK(
      serial.begin());

  serial.onJson(onJson);

  ESP_LOGI(
      TAG_LVGL_UI,
      "USB CDC serial initialized");

  while (true)
  {
    serial.process();

    vTaskDelay(
        pdMS_TO_TICKS(10));
  }
}