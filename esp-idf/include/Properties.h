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

// ----------------------
// HID
// ----------------------
#define PREVIOUS_BTN_PIN GPIO_NUM_4
#define PAUSE_BTN_PIN GPIO_NUM_5
#define NEXT_BTN_PIN GPIO_NUM_6
#define ENCODER_CLK_PIN GPIO_NUM_7
#define ENCODER_DT_PIN GPIO_NUM_15
#define ENCODER_SW_PIN GPIO_NUM_16

// ----------------------
// Led
// ----------------------
#define LED_PIN GPIO_NUM_48
#define LED_COUNT 1

// ----------------------
// Preferences namespace
// ----------------------
#define PREFERENCES_NAMESAPCE "DEVICE"