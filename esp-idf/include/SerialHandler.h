#pragma once

#include <ArduinoJson.h>

#include "JsonSerial.h"
#include "Validation.h"
#include "Led.h"

static const char *TAG_SERIAL_HANDLER = "SERIAL_HANDLER";

static JsonSerial serial(TINYUSB_CDC_ACM_0);

class MessageTypes
{
public:
    static constexpr const char *SET_LED = "SET_LED";
    static constexpr const char *GET_DATA = "GET_DATA";
};

static void onJson(JsonDocument &doc)
{
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
    else if (strcmp(type, MessageTypes::GET_DATA) == 0)
    {
        /* code */
    }
    else
    {
        return;
    }

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