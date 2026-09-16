#pragma once

#include <ArduinoJson.h>
#include "mbedtls/base64.h"
#include "esp_heap_caps.h"
#include "LvglUI.h"
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

static inline uint16_t rgb565_swap_rb(uint16_t v)
{
  return (uint16_t)((v & 0x07E0)             // keep green
                    | ((v & 0x001F) << 11)   // B → R
                    | ((v & 0xF800) >> 11)); // R → B
}

static inline uint16_t swap16(uint16_t v)
{
  return (uint16_t)((v << 8) | (v >> 8));
}

static bool decodeImageBase64(
    const char *base64,
    size_t base64_length)
{
  if (base64_length != 204800)
  {
    return false;
  }

  if (!image_buffer)
  {
    if (!image_buffer)
    {
      return false;
    }
  }

  size_t decoded_size = 0;

  int result = mbedtls_base64_decode(
      image_buffer,
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

const lv_image_dsc_t wallpaper_img_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_RGB565,
        .flags = 0,
        .w = 320,
        .h = 240,
        .stride = 320 * 2,
        .reserved_2 = 0,
    },
    .data_size = 153600,
    .data = image_buffer,
    .reserved = nullptr,
    .reserved_2 = 0,
};

static void onImage(const char *type, size_t typeLen,
                    const char *base64, size_t base64Len)
{
  if (!decodeImageBase64(base64, base64Len))
  {
    ESP_LOGE(TAG_SERIAL_HANDLER, "image decode failed");
    return;
  }

  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(
      panel_handle,
      0, 0,
      LCD_WIDTH, LCD_HEIGHT,
      image_buffer));

  wallpaper_img = lv_image_create(lv_screen_active());
  lv_image_set_src(wallpaper_img, &wallpaper_img_dsc);
  lv_obj_center(wallpaper_img);

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