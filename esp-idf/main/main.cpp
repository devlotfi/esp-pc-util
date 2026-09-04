#include "Properties.h"
#include "Gpio.h"
#include "TinyUsbHid.h"

static const char *TAG_MAIN = "GPIO";

extern "C" void app_main(void)
{
    setup_gpio();
    setup_tinyusb();

    BaseType_t task_result = xTaskCreate(
        tiny_usb_task,
        "tiny_usb",
        4096,
        nullptr,
        5,
        nullptr);
    if (task_result != pdPASS)
    {
        ESP_LOGE(TAG_MAIN, "Failed to create button task");
        abort();
    }
}
