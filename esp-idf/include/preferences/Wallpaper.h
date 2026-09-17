#pragma once

#include <stdint.h>
#include <cstring>
#include "esp_partition.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_log.h"
#include "Properties.h"

static const char *TAG_WALLPAPER = "wallpaper";

static constexpr uint32_t WALLPAPER_MAGIC = 0xCAFEBABE;

struct WallpaperData
{
  uint32_t magic; // WALLPAPER_MAGIC if valid, garbage/0xFFFFFFFF if never written
  bool isSet;
  uint8_t image[IMAGE_SIZE];
};

static constexpr size_t WALLPAPER_RECORD_SIZE = sizeof(WallpaperData);

static bool wallpaperDataCacheSet = false;
static WallpaperData *wallpaperDataCache = static_cast<WallpaperData *>(
    heap_caps_malloc(
        sizeof(WallpaperData),
        MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));

static lv_image_dsc_t wallpaper_img_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_RGB565_SWAPPED,
        .flags = 0,
        .w = IMAGE_WIDTH,
        .h = IMAGE_HEIGHT,
        .stride = IMAGE_WIDTH * 2,
        .reserved_2 = 0,
    },
    .data_size = IMAGE_SIZE,
    .data = wallpaperDataCache->image,
    .reserved = nullptr,
    .reserved_2 = 0,
};

static const esp_partition_t *getWallpaperPartition()
{
  static const esp_partition_t *part = nullptr;
  if (part == nullptr)
  {
    part = esp_partition_find_first(
        static_cast<esp_partition_type_t>(ESP_PARTITION_TYPE_DATA),
        static_cast<esp_partition_subtype_t>(0x40), // must match partitions.csv subtype
        "wallpaper");

    if (part == nullptr)
    {
      ESP_LOGE(TAG_WALLPAPER, "wallpaper partition not found - check partitions.csv");
    }
  }
  return part;
}

static void saveWallpaperData()
{
  if (!wallpaperDataCache)
    return;

  const esp_partition_t *part = getWallpaperPartition();
  if (!part)
    return;

  if (WALLPAPER_RECORD_SIZE > part->size)
  {
    ESP_LOGE(TAG_WALLPAPER, "record (%u) larger than partition (%u)",
             (unsigned)WALLPAPER_RECORD_SIZE, (unsigned)part->size);
    return;
  }

  // Flash writes can only clear bits (1->0); reverting to 1 requires an
  // erase first. Erase must be sector (4096-byte) aligned.
  size_t erase_size = (WALLPAPER_RECORD_SIZE + 4095) & ~((size_t)4095);
  if (erase_size > part->size)
    erase_size = part->size;

  esp_err_t err = esp_partition_erase_range(part, 0, erase_size);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG_WALLPAPER, "erase failed: %s", esp_err_to_name(err));
    return;
  }
  wallpaperDataCache->isSet = true;
  err = esp_partition_write(part, 0, wallpaperDataCache, WALLPAPER_RECORD_SIZE);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG_WALLPAPER, "write failed: %s", esp_err_to_name(err));
    return;
  }

  ESP_LOGI(TAG_WALLPAPER, "wallpaper saved (%u bytes)", (unsigned)WALLPAPER_RECORD_SIZE);
}

static WallpaperData *loadWallpaperData()
{
  if (wallpaperDataCache && wallpaperDataCacheSet)
    return wallpaperDataCache;

  if (!wallpaperDataCache)
    return nullptr;

  const esp_partition_t *part = getWallpaperPartition();
  if (!part)
  {
    wallpaperDataCache->magic = 0;
    wallpaperDataCacheSet = true;
    return wallpaperDataCache;
  }

  esp_err_t err = esp_partition_read(part, 0, wallpaperDataCache, WALLPAPER_RECORD_SIZE);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG_WALLPAPER, "read failed: %s", esp_err_to_name(err));
    wallpaperDataCache->magic = 0;
  }

  wallpaperDataCacheSet = true;
  return wallpaperDataCache;
}

static inline bool wallpaperIsValid(const WallpaperData *data)
{
  return data != nullptr && data->magic == WALLPAPER_MAGIC;
}