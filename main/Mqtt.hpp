#pragma once

#include <string>

#include "mqtt_client.h"

class Mqtt
{
public:
    Mqtt(std::string ip, std::string port);

    void Connect();
private:
    static constexpr char LOG_TAG[] = "MQTT";
    std::string m_ip;
    std::string m_port;
    esp_mqtt_client_handle_t m_client;

    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    static void log_error_if_nonzero(const char *message, int error_code);

};