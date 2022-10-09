#pragma once

#include <string>

#include "esp_event_base.h"

class WifiHandler
{
public:
    WifiHandler(std::string ssid, std::string pass);
    ~WifiHandler();
    
    void wifi_init_sta(void);
private:
    static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
    std::string m_ssid;
    std::string m_pass;

    static constexpr char EXAMPLE_ESP_WIFI_SSID[32] =  "Vainberger";
    static constexpr char EXAMPLE_ESP_WIFI_PASS[64] = "pedro1989";
    static constexpr int EXAMPLE_ESP_MAXIMUM_RETRY = 5;
    
    static constexpr char TAG[] = "WIFI";

    esp_event_handler_instance_t lost_ip;
};