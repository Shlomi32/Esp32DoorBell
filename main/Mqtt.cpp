#include <iostream>
#include <sstream>
#include <string>

#include "esp_log.h"

#include "Mqtt.hpp"

std::map<std::string, std::unique_ptr<Mqtt::TopicHandler>> Mqtt::m_handlers;

Mqtt::Mqtt(std::string ip, std::string port) : m_ip{ip}, m_port{port}
{

}

void Mqtt::log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(LOG_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void Mqtt::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(LOG_TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    std::string topic{event->topic, (size_t)event->topic_len};
    std::string data{event->data, (size_t)event->data_len};

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(LOG_TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(LOG_TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(LOG_TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(LOG_TAG, "sent unsubscribe successful, msg_id=%d", msg_id);

        for (auto &sub : m_handlers)
        {
            ESP_LOGI(LOG_TAG, "Subscribe for %s", sub.first.c_str());
            esp_mqtt_client_subscribe(client, sub.first.c_str(), 0);
        }

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(LOG_TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
                
        if (Mqtt::m_handlers.find(topic) != Mqtt::m_handlers.end())
        {
            ESP_LOGW(LOG_TAG,"Calling handler");
            Mqtt::m_handlers[topic]->handle(topic, data);
        }
        else
        {
            ESP_LOGW(LOG_TAG, "No handler to call %s", topic.c_str());
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(LOG_TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            Mqtt::log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            Mqtt::log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            Mqtt::log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(LOG_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(LOG_TAG, "Other event id:%d", event->event_id);
        break;
    }
}


void Mqtt::Connect()
{
    std::stringstream ss{};
    ss << "mqtt://" << m_ip << ":" << m_port;
    esp_mqtt_client_config_t mqtt_cfg = {};
    std::string uri = ss.str();
    mqtt_cfg.broker.address.uri = uri.c_str();

    ESP_LOGI(LOG_TAG, "Init with %s", mqtt_cfg.broker.address.uri);
    m_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    ESP_LOGI(LOG_TAG, "Register events");
    esp_mqtt_client_register_event(m_client, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID), mqtt_event_handler, &m_handlers);
    ESP_LOGI(LOG_TAG, "Start");
    esp_mqtt_client_start(m_client);
}

int Mqtt::Publish(std::string topic, std::string msg)
{
    if (!m_client)
    {
        ESP_LOGE(LOG_TAG, "Client not initialized");
        return 0;
    }
    
    return esp_mqtt_client_publish(m_client, topic.c_str(), msg.c_str(), msg.length(), 0, 0);
}

void Mqtt::Subscribe(std::string topic, std::unique_ptr<TopicHandler> handler)
{
    ESP_LOGI(LOG_TAG, "Register handler to %s", topic.c_str());
    m_handlers[topic] = std::move(handler);
}
