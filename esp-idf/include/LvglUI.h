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
#include "Vars.h"
#include "my_image.h"

static const char *TAG_LVGL_UI = "LVGL_UI";

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    if (lv_display != NULL)
    {
        lv_display_flush_ready(lv_display);
    }
    return false;
}

static void lvgl_flush_cb(
    lv_display_t *display,
    const lv_area_t *area,
    uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel =
        (esp_lcd_panel_handle_t)lv_display_get_user_data(display);

    ESP_ERROR_CHECK(
        esp_lcd_panel_draw_bitmap(
            panel,
            area->x1,
            area->y1,
            area->x2 + 1,
            area->y2 + 1,
            px_map));
}

static void ui()
{
    LV_IMG_DECLARE(my_image);

    lv_obj_t *screen =
        lv_screen_active();

    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(0xFFFFFF),
        0);

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &my_image);
    lv_obj_center(img);
}

static void lvgl_task(void *arg)
{
    ESP_LOGI(TAG_LVGL_UI, "Starting LVGL");

    lv_init();
    lv_display = lv_display_create(
        LCD_WIDTH,
        LCD_HEIGHT);
    if (lv_display == NULL)
    {
        ESP_LOGE(TAG_LVGL_UI, "Failed to create LVGL display");
        vTaskDelete(NULL);
        return;
    }

    lv_display_set_color_format(lv_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    uint16_t *draw_buffer =
        (uint16_t *)heap_caps_malloc(
            LCD_WIDTH *
                LVGL_BUFFER_LINES *
                sizeof(uint16_t),
            MALLOC_CAP_DMA);
    if (draw_buffer == NULL)
    {
        ESP_LOGE(TAG_LVGL_UI, "Failed to allocate LVGL buffer");
        vTaskDelete(NULL);
        return;
    }
    lv_display_set_buffers(
        lv_display,
        draw_buffer,
        NULL,
        LCD_WIDTH *
            LVGL_BUFFER_LINES *
            sizeof(uint16_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_user_data(
        lv_display,
        panel_handle);

    lv_display_set_flush_cb(
        lv_display,
        lvgl_flush_cb);

    // draw ui
    ui();

    ESP_LOGI(TAG_LVGL_UI, "LVGL initialized");

    while (true)
    {

        uint32_t delay_ms =
            lv_timer_handler();

        if (delay_ms < 1)
        {
            delay_ms = 1;
        }

        if (delay_ms > 20)
        {
            delay_ms = 20;
        }

        vTaskDelay(
            pdMS_TO_TICKS(delay_ms));
    }
}