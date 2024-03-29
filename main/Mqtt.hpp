#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "mqtt_client.h"

class Mqtt
{
public:
    class TopicHandler
    {
    public:
        virtual void handle(std::string topic, std::string msg) = 0;
    };

    struct mqttMsg
    {
        const char *topic = NULL;
        const char *data = NULL;
    };

    Mqtt(std::string ip, std::string port);
    ~Mqtt();

    void Connect();

    int Publish(mqttMsg msg, bool isISR = false);
    void Subscribe(std::string topic, std::unique_ptr<TopicHandler> handler);
private:
    static constexpr char LOG_TAG[] = "MQTT";
    static std::map<std::string, std::vector<std::unique_ptr<TopicHandler>>> m_handlers;
    std::string m_ip;
    std::string m_port;
    esp_mqtt_client_handle_t m_client = NULL;
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    static void log_error_if_nonzero(const char *message, int error_code);
    
    static void publisherTaskhandler(void *args);
    QueueHandle_t m_msgQueue;
    TaskHandle_t m_publisherTask;
};