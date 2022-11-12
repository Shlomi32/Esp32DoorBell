#include "Mqtt.hpp"

class TaskResumer : public Mqtt::TopicHandler
{
public:
    TaskResumer(TaskHandle_t *task) : m_task{task} 
    {}
  virtual void handle(std::string topic, std::string msg)
  {
    vTaskResume(*m_task);
  }
private:
    TaskHandle_t *m_task;
};