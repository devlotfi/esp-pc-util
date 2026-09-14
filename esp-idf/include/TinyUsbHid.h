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

enum
{
  ITF_NUM_HID = 0,
  ITF_NUM_CDC,      // CDC control (notification) interface
  ITF_NUM_CDC_DATA, // CDC data interface
  ITF_NUM_TOTAL
};

#define EPNUM_HID 0x81       // IN
#define EPNUM_CDC_NOTIF 0x82 // IN
#define EPNUM_CDC_OUT 0x02   // OUT
#define EPNUM_CDC_IN 0x83    // IN

static const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_CONSUMER(),
};

#define TUSB_DESCRIPTOR_TOTAL_LEN \
  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_CDC_DESC_LEN)

static const uint8_t hid_configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(
        1,                         // Configuration number
        ITF_NUM_TOTAL,             // Interface count -> now 3
        0,                         // Configuration string index
        TUSB_DESCRIPTOR_TOTAL_LEN, // Total descriptor length
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
        100),

    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID,
        4, // String index
        HID_ITF_PROTOCOL_NONE,
        sizeof(hid_report_descriptor),
        EPNUM_HID,
        16,
        10),

    TUD_CDC_DESCRIPTOR(
        ITF_NUM_CDC,
        5, // String index
        EPNUM_CDC_NOTIF, 8,
        EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),
};

static const uint8_t language_descriptor[] = {
    0x09,
    0x04,
};

static const char *hid_string_descriptor[] = {
    reinterpret_cast<const char *>(language_descriptor), // 0: English
    "Espressif",                                         // 1: Manufacturer
    "ESP32-S3 esp-pc-util",                              // 2: Product
    "000001",                                            // 3: Serial number
    "Consumer Control",                                  // 4: HID interface
    "JSON Serial",                                       // 5: CDC interface
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

void send_key(uint16_t report)
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

// --------------------------------------------------
// Rotary encoder
// --------------------------------------------------

struct RotaryEncoder
{
  uint8_t previous_state;
};

static RotaryEncoder encoder = {
    .previous_state = 0,
};

// Read CLK + DT as a 2-bit state
static inline uint8_t read_encoder_state(void)
{
  const uint8_t clk =
      gpio_get_level(ENCODER_CLK_PIN);
  const uint8_t dt =
      gpio_get_level(ENCODER_DT_PIN);
  return (clk << 1) | dt;
}

enum class ButtonEvents : uint8_t
{
  EVENT_PREVIOUS_TRACK,
  EVENT_PAUSE,
  EVENT_NEXT_TRACK,
  EVENT_MUTE,
  EVENT_NONE,
};

enum class EncoderEvents : uint8_t
{
  EVENT_VOLUME_UP,
  EVENT_VOLUME_DOWN,
  EVENT_NONE,
};

static volatile int8_t encoder_accumulator = 0;
static volatile EncoderEvents encoder_event = EncoderEvents::EVENT_NONE;
static volatile ButtonEvents button_event = ButtonEvents::EVENT_NONE;

static TaskHandle_t tiny_usb_task_handle = nullptr;

static void IRAM_ATTR button_isr_handler(void *arg)
{
  (void)arg;

  bool previous_pressed = gpio_get_level(PREVIOUS_BTN_PIN) == 0;
  bool pause_pressed = gpio_get_level(PAUSE_BTN_PIN) == 0;
  bool next_pressed = gpio_get_level(NEXT_BTN_PIN) == 0;
  bool mute_pressed = gpio_get_level(ENCODER_SW_PIN) == 0;

  if (previous_pressed + pause_pressed + next_pressed + mute_pressed == 1)
  {
    if (previous_pressed)
    {
      button_event = ButtonEvents::EVENT_PREVIOUS_TRACK;
    }
    else if (pause_pressed)
    {
      button_event = ButtonEvents::EVENT_PAUSE;
    }
    else if (next_pressed)
    {
      button_event = ButtonEvents::EVENT_NEXT_TRACK;
    }
    else if (mute_pressed)
    {
      button_event = ButtonEvents::EVENT_MUTE;
    }
  }
  else
  {
    button_event = ButtonEvents::EVENT_NONE;
  }

  BaseType_t higher_priority_task_woken = pdFALSE;

  if (tiny_usb_task_handle != nullptr)
  {
    vTaskNotifyGiveFromISR(
        tiny_usb_task_handle,
        &higher_priority_task_woken);
  }

  if (higher_priority_task_woken)
  {
    portYIELD_FROM_ISR();
  }
}

static void IRAM_ATTR encoder_isr_handler(void *arg)
{
  (void)arg;

  const uint8_t current_state =
      ((uint8_t)gpio_get_level(ENCODER_CLK_PIN) << 1) |
      (uint8_t)gpio_get_level(ENCODER_DT_PIN);

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
    encoder_accumulator = encoder_accumulator + 1;
    break;

  case 0b0010:
  case 0b1011:
  case 0b1101:
  case 0b0100:
    encoder_accumulator = encoder_accumulator - 1;
    break;

  default:
    break;
  }

  if (encoder_accumulator >= 4)
  {
    encoder_accumulator = 0;
    encoder_event = EncoderEvents::EVENT_VOLUME_UP;
  }
  else if (encoder_accumulator <= -4)
  {
    encoder_accumulator = 0;
    encoder_event = EncoderEvents::EVENT_VOLUME_DOWN;
  }
  else
  {
    return;
  }

  BaseType_t higher_priority_task_woken = pdFALSE;

  if (tiny_usb_task_handle != nullptr)
  {
    vTaskNotifyGiveFromISR(
        tiny_usb_task_handle,
        &higher_priority_task_woken);
  }

  if (higher_priority_task_woken)
  {
    portYIELD_FROM_ISR();
  }
}

static void tiny_usb_task(void *arg)
{
  (void)arg;

  // Establish initial encoder state before enabling interrupts
  encoder.previous_state = read_encoder_state();

  tiny_usb_task_handle = xTaskGetCurrentTaskHandle();

  // Install GPIO ISR service
  ESP_ERROR_CHECK(
      gpio_install_isr_service(ESP_INTR_FLAG_IRAM));

  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          PREVIOUS_BTN_PIN,
          button_isr_handler,
          nullptr));
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          PAUSE_BTN_PIN,
          button_isr_handler,
          nullptr));
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          NEXT_BTN_PIN,
          button_isr_handler,
          nullptr));
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          ENCODER_SW_PIN,
          button_isr_handler,
          nullptr));

  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          ENCODER_CLK_PIN,
          encoder_isr_handler,
          nullptr));
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(
          ENCODER_DT_PIN,
          encoder_isr_handler,
          nullptr));

  while (true)
  {
    if (ulTaskNotifyTake(pdTRUE, 0) > 0)
    {
      if (encoder_event == EncoderEvents::EVENT_VOLUME_UP)
      {
        send_key(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
      }
      else if (encoder_event == EncoderEvents::EVENT_VOLUME_DOWN)
      {
        send_key(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
      }

      if (button_event == ButtonEvents::EVENT_PREVIOUS_TRACK)
      {
        send_key(HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK);
      }
      else if (button_event == ButtonEvents::EVENT_PAUSE)
      {
        send_key(HID_USAGE_CONSUMER_PLAY_PAUSE);
      }
      else if (button_event == ButtonEvents::EVENT_NEXT_TRACK)
      {
        send_key(HID_USAGE_CONSUMER_SCAN_NEXT_TRACK);
      }
      else if (button_event == ButtonEvents::EVENT_MUTE)
      {
        send_key(HID_USAGE_CONSUMER_MUTE);
      }

      button_event = ButtonEvents::EVENT_NONE;
      encoder_event = EncoderEvents::EVENT_NONE;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}