/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
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
gpio_num_t PRIMARY_GARAGE_DOOR_RELAY = GPIO_NUM_21;
gpio_num_t SECONDARY_GARAGE_DOOR_RELAY = GPIO_NUM_22;

TaskHandle_t keyPressedTask = NULL;
TaskHandle_t gateOpenerTask = NULL;
TaskHandle_t PrimaryGarageOpenerTask = NULL;
TaskHandle_t SecondaryGarageOpenerTask = NULL;
TaskHandle_t mqttPublisherTask = NULL;

QueueHandle_t mqttPublisherQueue = NULL;

std::unique_ptr<Mqtt> mqttClient = NULL;

void KeyPressed(void *args)
{
  while (1)
  {
    vTaskSuspend(NULL);
    Mqtt::mqttMsg msg = {.topic="/garden/doorBell", .data="Pressed"};
    ESP_LOGI(LOG_TAG, "Publish mqtt msg %s - %s", msg.topic, msg.data);
    mqttClient->Publish(msg, true);

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
    Mqtt::mqttMsg msg = {.topic="/garden/openGate/ack", .data=""};
    mqttClient->Publish(msg);
    relay.SetLevel(1);

    vTaskDelay((DOOR_OPEN_TIME * 1000) / portTICK_PERIOD_MS);
    relay.SetLevel(0);
    ESP_LOGI(LOG_TAG, "Door locked");
  }
}

void isr_handler(void *args)
{
  TaskHandle_t *mqttPushlisher = static_cast<TaskHandle_t *>(args);
  xTaskResumeFromISR(*mqttPushlisher);
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

    mqttClient = std::make_unique<Mqtt>("192.168.1.14", "1883");

    xTaskCreate(KeyPressed, "DoorOpenRequest", 8192, NULL, 2, &keyPressedTask);
    xTaskCreate(GateOpener, "GateOpener", 2048, &DOOR_RELAY, 1, &gateOpenerTask);
    xTaskCreate(GateOpener, "PrimaryGarageOpener", 2048, &PRIMARY_GARAGE_DOOR_RELAY, 1, &PrimaryGarageOpenerTask);
    xTaskCreate(GateOpener, "SecondaryGarageDoor", 2048, &SECONDARY_GARAGE_DOOR_RELAY, 1, &SecondaryGarageOpenerTask);
    
    static auto btn = std::make_unique<GpioInput>(BTN);
    btn->SetIsrhandler(isr_handler, &keyPressedTask);
    
    mqttClient->Subscribe("/garden/openGate/Walking", std::make_unique<TaskResumer>(&gateOpenerTask));
    mqttClient->Subscribe("/garden/openGate/Primary", std::make_unique<TaskResumer>(&PrimaryGarageOpenerTask));
    mqttClient->Subscribe("/garden/openGate/Secondary", std::make_unique<TaskResumer>(&SecondaryGarageOpenerTask));
    mqttClient->Connect();
}
