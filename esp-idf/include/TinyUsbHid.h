#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tusb.h"
#include "class/hid/hid.h"
#include "Properties.h"

static const char *TAG_TINY_USB = "TINY_USB";

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_CONSUMER(),
};

#define TUSB_DESCRIPTOR_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

static const uint8_t hid_configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,                         // Configuration number
        1,                         // Interface count
        0,                         // Configuration string index
        TUSB_DESCRIPTOR_TOTAL_LEN, // Total descriptor length
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
        100 // Max power: 100 mA
        ),
    TUD_HID_DESCRIPTOR(
        0,                     // Interface number
        4,                     // String index
        HID_ITF_PROTOCOL_NONE, // Non-boot HID
        sizeof(hid_report_descriptor),
        0x81, // Interrupt IN endpoint
        16,   // Endpoint size
        10    // Polling interval: 10 ms
        ),
};

static const uint8_t language_descriptor[] = {
    0x09,
    0x04};

static const char *hid_string_descriptor[] = {
    reinterpret_cast<const char *>(language_descriptor), // 0: English
    "Espressif",                                         // 1: Manufacturer
    "ESP32-S3 esp-pc-util",                              // 2: Product
    "000001",                                            // 3: Serial number
    "Consumer Control"                                   // 4: HID interface
};

extern "C" uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
  (void)instance;
  return hid_report_descriptor;
}

extern "C" uint16_t tud_hid_get_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t *buffer,
    uint16_t reqlen)
{
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}

extern "C" void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize)
{
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}

void setup_tinyusb(void)
{
  tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
  tusb_cfg.descriptor.full_speed_config =
      hid_configuration_descriptor;
  tusb_cfg.descriptor.string =
      hid_string_descriptor;
  tusb_cfg.descriptor.string_count =
      sizeof(hid_string_descriptor) /
      sizeof(hid_string_descriptor[0]);
  ESP_ERROR_CHECK(
      tinyusb_driver_install(&tusb_cfg));
}

void send_key(uint16_t &report)
{
  if (!tud_mounted())
  {
    ESP_LOGW(TAG_TINY_USB, "USB device is not mounted");
    return;
  }
  if (!tud_hid_ready())
  {
    ESP_LOGW(TAG_TINY_USB, "USB HID not ready");
    return;
  }

  if (!tud_hid_report(
          0,
          &report,
          sizeof(report)))
  {

    ESP_LOGW(TAG_TINY_USB, "Failed to send action");
    return;
  }
  ESP_LOGI(TAG_TINY_USB, "Action pressed");

  vTaskDelay(pdMS_TO_TICKS(20));

  uint16_t release = 0;
  if (!tud_hid_report(
          0,
          &release,
          sizeof(release)))
  {

    ESP_LOGW(TAG_TINY_USB, "Failed to send release action");
    return;
  }
  ESP_LOGI(TAG_TINY_USB, "Action released");
}

struct PinInput
{
  gpio_num_t pin;
  uint16_t key;
  bool previous_pressed;
};

static PinInput pinInputs[] = {
    {
        .pin = PREVIOUS_BTN_PIN,
        .key = HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK,
        .previous_pressed = false,
    },
    {
        .pin = PAUSE_BTN_PIN,
        .key = HID_USAGE_CONSUMER_PLAY_PAUSE,
        .previous_pressed = false,
    },
    {
        .pin = NEXT_BTN_PIN,
        .key = HID_USAGE_CONSUMER_SCAN_NEXT_TRACK,
        .previous_pressed = false,
    },
    {
        .pin = ENCODER_SW_PIN,
        .key = HID_USAGE_CONSUMER_MUTE,
        .previous_pressed = false,
    },
};

struct RotaryEncoder
{
  uint8_t previous_state;
  bool previous_switch_pressed;
};
static RotaryEncoder encoder = {
    .previous_state = 0,
    .previous_switch_pressed = false,
};

static uint8_t read_encoder_state(void)
{
  const uint8_t clk =
      gpio_get_level(ENCODER_CLK_PIN);
  const uint8_t dt =
      gpio_get_level(ENCODER_DT_PIN);
  return (clk << 1) | dt;
}

static int8_t encoder_accumulator = 0;

static void process_encoder(void)
{
  const uint8_t current_state =
      read_encoder_state();
  const uint8_t transition =
      (encoder.previous_state << 2) |
      current_state;
  encoder.previous_state = current_state;

  switch (transition)
  {
  case 0b0001:
  case 0b0111:
  case 0b1110:
  case 0b1000:
    encoder_accumulator++;
    break;
  case 0b0010:
  case 0b1011:
  case 0b1101:
  case 0b0100:
    encoder_accumulator--;
    break;
  default:
    break;
  }

  if (encoder_accumulator >= 4)
  {
    encoder_accumulator = 0;

    uint16_t report = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
    send_key(report);
  }
  else if (encoder_accumulator <= -4)
  {
    encoder_accumulator = 0;

    uint16_t report = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
    send_key(report);
  }
}

static void tiny_usb_task(void *arg)
{
  (void)arg;
  encoder.previous_state = read_encoder_state();
  while (true)
  {
    for (auto &pinInput : pinInputs)
    {
      const bool pressed =
          gpio_get_level(pinInput.pin) == 0;

      if (pressed && !pinInput.previous_pressed)
      {
        send_key(pinInput.key);
        vTaskDelay(pdMS_TO_TICKS(50));
      }
      pinInput.previous_pressed = pressed;
    }
    process_encoder();
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}