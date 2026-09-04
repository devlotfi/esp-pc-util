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
// TFT LCD
// ----------------------
#define LCD_HOST SPI2_HOST
#define LCD_SCLK GPIO_NUM_12
#define LCD_MOSI GPIO_NUM_11
#define LCD_MISO GPIO_NUM_13
#define LCD_CS GPIO_NUM_10
#define LCD_DC GPIO_NUM_9
#define LCD_RST GPIO_NUM_8
#define LCD_BL GPIO_NUM_18
#define LCD_WIDTH 320
#define LCD_HEIGHT 240

// ----------------------
// LVGL
// ----------------------
#define LVGL_BUFFER_LINES 80

// ----------------------
// Led
// ----------------------
#define LED_PIN GPIO_NUM_48
#define LED_COUNT 1

// ----------------------
// Preferences namespace
// ----------------------
#define PREFERENCES_NAMESAPCE "DEVICE"