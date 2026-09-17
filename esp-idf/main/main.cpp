#include "Properties.h"
#include "Vars.h"
#include "Gpio.h"
#include "TinyUsbHid.h"
#include "TftLcd.h"
#include "LvglUI.h"
#include "SpiRamAllocator.h"
#include "JsonSerial.h"
#include "SerialHandler.h"
#include "Validation.h"
#include "Led.h"
#include "NvsStorage.h"
#include "preferences/Led.h"
#include "preferences/Wallpaper.h"

static const char *TAG_MAIN = "GPIO";

extern "C" void app_main(void)
{
    setup_gpio();
    setup_nvs();
    setup_led();
    setup_tinyusb();
    setup_tft_lcd();
    setup_tft_brightness();
    set_tft_brightness(100);

    BaseType_t task_result_tiny_usb = xTaskCreate(
        tiny_usb_task,
        "tiny_usb",
        4096,
        nullptr,
        5,
        nullptr);
    if (task_result_tiny_usb != pdPASS)
    {
        ESP_LOGE(TAG_TINY_USB, "Failed to create button task");
        abort();
    }

    BaseType_t task_result_lvgl =
        xTaskCreate(
            lvgl_task,
            "lvgl",
            8192,
            NULL,
            5,
            NULL);

    if (task_result_lvgl != pdPASS)
    {
        ESP_LOGE(TAG_LVGL_UI, "Failed to create LVGL task");
        return;
    }

    BaseType_t task_result_serial_handler = xTaskCreate(
        serial_handler_task,
        "serial_handler",
        4096,
        nullptr,
        5,
        nullptr);
    if (task_result_serial_handler != pdPASS)
    {
        ESP_LOGE(TAG_TINY_USB, "Failed to create serial handler task");
        abort();
    }
}
