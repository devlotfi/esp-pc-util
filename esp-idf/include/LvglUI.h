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
#include "esp_timer.h"
#include "lvgl.h"
#include "Properties.h"
#include "Vars.h"

static const char *TAG_LVGL_UI = "LVGL_UI";

static volatile uint32_t flush_ready_count = 0;
static void lvgl_tick_timer_cb(void *arg)
{
  lv_tick_inc(5); // must match the timer period below
}

static esp_timer_handle_t lvgl_tick_timer = nullptr;

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
  flush_ready_count = flush_ready_count + 1;
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
  ESP_LOGI(TAG_LVGL_UI, "flush_cb: (%d,%d)-(%d,%d), flush_ready_count=%lu",
           area->x1, area->y1, area->x2, area->y2, flush_ready_count);
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
  lv_obj_t *screen =
      lv_screen_active();

  lv_obj_set_style_bg_color(
      screen,
      lv_color_hex(0xFF00FF),
      0);
}

static void lvgl_task(void *arg)
{
  ESP_LOGI(TAG_LVGL_UI, "Starting LVGL");

  lv_init();

  esp_timer_create_args_t lvgl_tick_timer_args = {};
  lvgl_tick_timer_args.callback = &lvgl_tick_timer_cb;
  lvgl_tick_timer_args.name = "lvgl_tick";
  ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 5 * 1000)); // 5ms in µs

  lv_display = lv_display_create(
      LCD_WIDTH,
      LCD_HEIGHT);

  if (lv_display == NULL)
  {
    ESP_LOGE(TAG_LVGL_UI, "Failed to create LVGL display");
    vTaskDelete(NULL);
    return;
  }

  lv_display_set_color_format(
      lv_display,
      LV_COLOR_FORMAT_RGB565_SWAPPED);

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

  lv_obj_t *screen = lv_screen_active();

  ESP_LOGI(TAG_LVGL_UI, "LVGL initialized");

  uint32_t colors[] = {
      0xFF0000, // red
      0x00FF00, // green
      0x0000FF, // blue
      0xFFFFFF, // white
      0x000000  // black
  };

  int color_index = 0;

  while (true)
  {
    color_index++;
    ESP_LOGI(TAG_LVGL_UI, "Setting color %d", color_index);

    lv_obj_set_style_bg_color(
        screen,
        lv_color_hex(colors[color_index]),
        0);

    uint32_t delay_ms = lv_timer_handler();

    if (delay_ms < 1)
    {
      delay_ms = 1;
    }

    if (delay_ms > 20)
    {
      delay_ms = 20;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}