#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"
#include "lvgl.h"
#include "Properties.h"
#include "SpiRamAllocator.h"

static QueueHandle_t lvgl_message_queue_handle = nullptr;
static TaskHandle_t tiny_usb_task_handle = nullptr;
static TaskHandle_t lvgl_task_handle = nullptr;
static TaskHandle_t serial_handler_task_handle = nullptr;
static esp_lcd_panel_handle_t panel_handle = nullptr;
static esp_lcd_panel_io_handle_t io_handle = nullptr;
static lv_display_t *lv_display = nullptr;
static SpiRamAllocator spiRamAllocator;

static constexpr size_t IMAGE_WIDTH = 320;
static constexpr size_t IMAGE_HEIGHT = 240;
static constexpr size_t IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 2;