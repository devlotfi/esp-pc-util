#pragma once

#include "nvs_flash.h"
#include "esp_err.h"

static nvs_handle_t preferences = 0;

void setup_nvs()
{
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(
        nvs_open(
            "settings",
            NVS_READWRITE,
            &preferences));
}