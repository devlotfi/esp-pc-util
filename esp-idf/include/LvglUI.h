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
#include "Utils.h"
#include "preferences/Wallpaper.h"
#include "preferences/Display.h"

static const char *TAG_LVGL_UI = "LVGL_UI";

// ============================================================
// Dashboard
// 320 x 240
// ============================================================

static lv_obj_t *wallpaper = nullptr;
static lv_obj_t *cpu_gauge;
static lv_obj_t *ram_gauge;
static lv_obj_t *cpu_value_label;
static lv_obj_t *ram_value_label;

// ------------------------------------------------------------
// Create a single card
// ------------------------------------------------------------

static lv_obj_t *create_card(
    lv_obj_t *parent,
    const char *title,
    int value,
    lv_obj_t **gauge_out,
    lv_obj_t **value_label_out)
{
  // Card
  lv_obj_t *card = lv_obj_create(parent);

  lv_obj_set_size(card, 135, 155);

  lv_obj_set_style_radius(card, 16, 0);

  // Semi-transparent background
  lv_obj_set_style_bg_color(
      card,
      lv_color_hex(0x1E293B),
      0);

  lv_obj_set_style_bg_opa(
      card,
      LV_OPA_80,
      0);

  // Border
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_color(
      card,
      lv_color_hex(0x64748B),
      0);

  lv_obj_set_style_border_opa(
      card,
      LV_OPA_50,
      0);

  lv_obj_set_style_pad_all(card, 0, 0);

  lv_obj_remove_flag(
      card,
      LV_OBJ_FLAG_SCROLLABLE);

  // --------------------------------------------------------
  // Title
  // --------------------------------------------------------

  lv_obj_t *title_label = lv_label_create(card);

  lv_label_set_text(title_label, title);

  lv_obj_set_style_text_color(
      title_label,
      lv_color_hex(0xCBD5E1),
      0);

  lv_obj_set_style_text_font(
      title_label,
      &lv_font_montserrat_14,
      0);

  lv_obj_align(
      title_label,
      LV_ALIGN_TOP_MID,
      0,
      10);

  // --------------------------------------------------------
  // Gauge
  // --------------------------------------------------------

  lv_obj_t *gauge = lv_arc_create(card);

  lv_obj_set_size(gauge, 115, 115);

  lv_obj_align(
      gauge,
      LV_ALIGN_TOP_MID,
      0,
      38);

  lv_arc_set_range(gauge, 0, 100);
  lv_arc_set_value(gauge, value);

  lv_arc_set_rotation(gauge, 135);
  lv_arc_set_bg_angles(gauge, 0, 270);

  // Background arc
  lv_obj_set_style_arc_color(
      gauge,
      lv_color_hex(0x3f5068),
      LV_PART_MAIN);

  lv_obj_set_style_arc_width(
      gauge,
      10,
      LV_PART_MAIN);

  // Indicator
  DisplayData *displayData = loadDisplayData();
  lv_obj_set_style_arc_color(
      gauge,
      lv_color_hex(hexColor(displayData->accent_color)),
      LV_PART_INDICATOR);

  lv_obj_set_style_arc_width(
      gauge,
      10,
      LV_PART_INDICATOR);

  // No knob
  lv_obj_remove_style(
      gauge,
      NULL,
      LV_PART_KNOB);

  // --------------------------------------------------------
  // Value
  // --------------------------------------------------------

  lv_obj_t *value_label = lv_label_create(card);

  lv_label_set_text_fmt(
      value_label,
      "%d%%",
      value);

  lv_obj_set_style_text_color(
      value_label,
      lv_color_hex(0xF8FAFC),
      0);

  lv_obj_set_style_text_font(
      value_label,
      &lv_font_montserrat_20,
      0);

  lv_obj_align(
      value_label,
      LV_ALIGN_TOP_MID,
      0,
      82);

  *gauge_out = gauge;
  *value_label_out = value_label;

  return card;
}

// ------------------------------------------------------------
// Create dashboard
// ------------------------------------------------------------

static void create_dashboard(lv_obj_t *parent)
{
  // Background
  lv_obj_set_style_bg_color(
      parent,
      lv_color_hex(0x0F172A),
      0);

  lv_obj_set_style_bg_opa(
      parent,
      LV_OPA_COVER,
      0);

  // CPU card
  lv_obj_t *cpu_card = create_card(
      parent,
      "CPU",
      42,
      &cpu_gauge,
      &cpu_value_label);

  lv_obj_align(
      cpu_card,
      LV_ALIGN_LEFT_MID,
      18,
      0);

  // RAM card
  lv_obj_t *ram_card = create_card(
      parent,
      "RAM",
      68,
      &ram_gauge,
      &ram_value_label);

  lv_obj_align(
      ram_card,
      LV_ALIGN_RIGHT_MID,
      -18,
      0);
}

static void ui()
{
  WallpaperData *wallpaperData = loadWallpaperData();

  lv_obj_t *screen =
      lv_screen_active();

  lv_obj_set_style_bg_color(
      screen,
      lv_color_hex(0xFF00FF),
      0);

  wallpaper = lv_image_create(screen);
  lv_obj_set_pos(wallpaper, 0, 0);
  if (wallpaperData->isSet)
  {
    lv_image_set_src(wallpaper, &wallpaper_img_dsc);
  }

  create_dashboard(lv_screen_active());
}