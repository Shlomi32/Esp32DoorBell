#include "esp_log.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"


#include "ChipInfo.hpp"

ChipInfo::ChipInfo() {
    esp_chip_info(&chip_info);
}


void ChipInfo::PrintInfo()
{
    uint32_t flash_size;

    ESP_LOGI(TAG,"This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    ESP_LOGI(TAG,"silicon revision %d, ", chip_info.revision);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    ESP_LOGI(TAG,"%uMB %s flash\n", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    ESP_LOGI(TAG,"Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
}