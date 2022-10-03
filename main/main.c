#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
static const int LED_PIN = 2;
void toggleLed(void *params)
{
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction (LED_PIN,GPIO_MODE_OUTPUT);
    while(1)
    {
        gpio_set_level(LED_PIN,0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN,1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    xTaskCreatePinnedToCore(toggleLed,
                            "Led_blink",
                            1024,
                            NULL,
                            1,
                            NULL,
                            1);
}