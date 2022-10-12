#pragma once

#include <string>
#include <memory>

#include "esp_event_base.h"

class WifiHandler
{
public:
    static void Create(std::string ssid, std::string pass)
    {
        instance = new WifiHandler(ssid, pass);
    }
    static WifiHandler &GetInstance()
    {
        return *instance;
    }
    ~WifiHandler();

    void wifi_init_sta(void);
protected:
    WifiHandler(std::string ssid, std::string pass);

private:
    static WifiHandler *instance;
    static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
    std::string m_ssid;
    std::string m_pass;

    static constexpr int EXAMPLE_ESP_MAXIMUM_RETRY = 5;
    
    static constexpr char TAG[] = "WIFI";

    esp_event_handler_instance_t lost_ip;

    static void wpa2_enterprise_example_task(void *pvParameters);
};