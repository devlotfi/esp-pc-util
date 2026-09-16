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

static const char *TAG_GPIO = "GPIO";

void setup_gpio()
{
  gpio_config_t previous_btn_config = {
      .pin_bit_mask = 1ULL << PREVIOUS_BTN_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&previous_btn_config));

  gpio_config_t pause_btn_config = {
      .pin_bit_mask = 1ULL << PAUSE_BTN_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&pause_btn_config));

  gpio_config_t next_btn_config = {
      .pin_bit_mask = 1ULL << NEXT_BTN_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&next_btn_config));

  // Rotary encoder CLK
  gpio_config_t encoder_clk_config = {
      .pin_bit_mask = 1ULL << ENCODER_CLK_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&encoder_clk_config));

  // Rotary encoder DT
  gpio_config_t encoder_dt_config = {
      .pin_bit_mask = 1ULL << ENCODER_DT_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&encoder_dt_config));

  // Encoder push button remains unchanged
  gpio_config_t encoder_sw_config = {
      .pin_bit_mask = 1ULL << ENCODER_SW_PIN,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&encoder_sw_config));
}