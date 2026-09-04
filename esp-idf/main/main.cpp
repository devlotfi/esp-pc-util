#include "Properties.h"
#include "Vars.h"
#include "Gpio.h"
#include "TinyUsbHid.h"
#include "TftLcd.h"
#include "LvglUI.h"
// #include "my_image.h"

static const char *TAG_MAIN = "GPIO";

extern "C" void app_main(void)
{
    setup_gpio();
    setup_tinyusb();
    setup_tft_lcd();
    setup_tft_brightness();
    set_tft_brightness(100);

    BaseType_t task_result = xTaskCreate(
        tiny_usb_task,
        "tiny_usb",
        4096,
        nullptr,
        5,
        nullptr);
    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG_TINY_USB, "Failed to create button task");
        abort();
    }

    BaseType_t result =
        xTaskCreate(
            lvgl_task,
            "lvgl",
            8192,
            NULL,
            5,
            NULL);

    if (result != pdPASS)
    {
        ESP_LOGE(TAG_LVGL_UI, "Failed to create LVGL task");
        return;
    }
}
