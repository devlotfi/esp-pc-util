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

static esp_lcd_panel_handle_t panel_handle;
static esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_display_t *lv_display = NULL;
static SpiRamAllocator spiRamAllocator;

static constexpr size_t IMAGE_WIDTH = 320;
static constexpr size_t IMAGE_HEIGHT = 240;
static constexpr size_t IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 2;
static uint8_t *image_buffer = static_cast<uint8_t *>(
    heap_caps_malloc(
        IMAGE_SIZE,
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));