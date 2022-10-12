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
#include "driver/gpio.h"
#include <memory>

#include "Wifi.hpp"
#include "Mqtt.hpp"
#include "ChipInfo.hpp"
#include "GpioInput.hpp"
#include "GpioOutput.hpp"
#include "MqttHandlers.hpp"

#define LOG_TAG "MAIN"
#define BTN GPIO_NUM_13

#define DOOR_OPEN_TIME 3
gpio_num_t DOOR_RELAY = GPIO_NUM_23;

TaskHandle_t mqttPublisherTask = NULL;
TaskHandle_t gateOpenerTask = NULL;
auto mqttClient = std::make_shared<Mqtt>("192.168.1.14", "1883");

void KeyPressed(void *args)
{
  // auto mqttClient = static_cast<Mqtt *>(args);
  while (1)
  {
    vTaskSuspend(NULL);
    
    ESP_LOGI(LOG_TAG, "Publish mqtt msg");
    mqttClient->Publish("/doorBell/button", "Pressed");

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void GateOpener(void *args)
{
  GpioOutput relay(*(static_cast<gpio_num_t *>(args)));
  while (1)
  {
    vTaskSuspend(NULL);

    ESP_LOGI(LOG_TAG, "Open door");
    relay.SetLevel(1);

    vTaskDelay((DOOR_OPEN_TIME * 1000) / portTICK_PERIOD_MS);
    relay.SetLevel(0);
    ESP_LOGI(LOG_TAG, "Door locked");
  }
}

void isr_handler(void *args)
{
  TaskHandle_t mqttPushlisher = static_cast<TaskHandle_t>(args);
  xTaskResumeFromISR(mqttPushlisher);
}

extern "C" void app_main(void)
{
    ESP_LOGI(LOG_TAG, "App startup");

    ChipInfo().PrintInfo();

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Connect to wifi
    WifiHandler::Create("Vainberger", "pedro1989");
    WifiHandler::GetInstance().wifi_init_sta();    

    xTaskCreate(KeyPressed, "DoorOpenRequest", 4096, mqttClient.get(), 2, &mqttPublisherTask);
    xTaskCreate(GateOpener, "GateOpener", 2024, &DOOR_RELAY, 1, &gateOpenerTask);
    
    auto btn = std::make_unique<GpioInput>(BTN);
    btn->SetIsrhandler(isr_handler, mqttPublisherTask);
    
    auto mqttClient = std::make_shared<Mqtt>("192.168.1.14", "1883");
    mqttClient->Subscribe("/garden/openGate", std::make_unique<TaskResumer>(&gateOpenerTask));
    mqttClient->Connect();
}
