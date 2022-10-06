/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "Wifi.hpp"
#include "Mqtt.hpp"
#include "ChipInfo.hpp"

#define LOG_TAG "MAIN"
Mqtt mqttClient("192.168.1.14", "1883");
WifiHandler wifi("Vainberger", "pedro1989");

extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Hello world");

    esp_log_level_set("*", ESP_LOG_INFO);

    ChipInfo info;

    info.PrintInfo();

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    wifi.wifi_init_sta();
    mqttClient.Connect();

    // vTaskDelay(2000);
}
