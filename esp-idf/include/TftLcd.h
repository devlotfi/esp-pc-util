#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"
#include "Properties.h"
#include "Vars.h"
#include "LvglUI.h"

static const char *TAG_TFT_LCD = "TFT_LCD";

void setup_tft_lcd()
{
    ESP_LOGI(TAG_TFT_LCD, "Initializing SPI bus");

    spi_bus_config_t bus_config = {};
    bus_config.sclk_io_num = LCD_SCLK;
    bus_config.mosi_io_num = LCD_MOSI;
    bus_config.miso_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.quadwp_io_num = -1;
    bus_config.max_transfer_sz = LCD_WIDTH * LVGL_BUFFER_LINES * sizeof(uint16_t);

    ESP_ERROR_CHECK(
        spi_bus_initialize(
            LCD_HOST,
            &bus_config,
            SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG_TFT_LCD, "Initializing LCD SPI IO");

    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.cs_gpio_num = LCD_CS;
    io_config.dc_gpio_num = LCD_DC;
    io_config.pclk_hz = 40 * 1000 * 1000;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    io_config.spi_mode = 0;
    io_config.trans_queue_depth = 10;
    io_config.on_color_trans_done = notify_lvgl_flush_ready;
    io_config.user_ctx = NULL;
    ESP_ERROR_CHECK(
        esp_lcd_new_panel_io_spi(
            (esp_lcd_spi_bus_handle_t)LCD_HOST,
            &io_config,
            &io_handle));

    ESP_LOGI(TAG_TFT_LCD, "Initializing ILI9341");

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
    panel_config.bits_per_pixel = 16;
    panel_config.reset_gpio_num = LCD_RST;

    ESP_ERROR_CHECK(
        esp_lcd_new_panel_ili9341(
            io_handle,
            &panel_config,
            &panel_handle));

    ESP_ERROR_CHECK(
        esp_lcd_panel_reset(panel_handle));

    ESP_ERROR_CHECK(
        esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(
        esp_lcd_panel_swap_xy(panel_handle, true));

    ESP_ERROR_CHECK(
        esp_lcd_panel_mirror(
            panel_handle,
            false,
            false));

    ESP_ERROR_CHECK(
        esp_lcd_panel_disp_on_off(
            panel_handle,
            true));

    ESP_LOGI(TAG_TFT_LCD, "ILI9341 initialized");
}

void setup_tft_brightness()
{
    ledc_timer_config_t tft_bl_timer = {};
    tft_bl_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    tft_bl_timer.duty_resolution = LEDC_TIMER_10_BIT;
    tft_bl_timer.timer_num = LEDC_TIMER_0;
    tft_bl_timer.freq_hz = 5000;
    tft_bl_timer.clk_cfg = LEDC_AUTO_CLK;

    ledc_channel_config_t tft_bl_channel = {};
    tft_bl_channel.gpio_num = LCD_BL;
    tft_bl_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    tft_bl_channel.channel = LEDC_CHANNEL_0;
    tft_bl_channel.intr_type = LEDC_INTR_DISABLE;
    tft_bl_channel.timer_sel = LEDC_TIMER_0;
    tft_bl_channel.duty = 0;
    tft_bl_channel.hpoint = 0;

    // Configure PWM timer
    ESP_ERROR_CHECK(
        ledc_timer_config(&tft_bl_timer));

    // Configure GPIO18 as PWM output
    ESP_ERROR_CHECK(
        ledc_channel_config(&tft_bl_channel));
}

void set_tft_brightness(uint8_t percentage)
{
    // Clamp to 0-100%
    if (percentage > 100)
        percentage = 100;

    // 10-bit PWM: 0-1023
    uint32_t duty = (percentage * 1023) / 100;

    ESP_ERROR_CHECK(
        ledc_set_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0,
            duty));

    ESP_ERROR_CHECK(
        ledc_update_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0));
}