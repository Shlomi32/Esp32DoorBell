#pragma once

#include "Gpio.hpp"

class GpioInput : public Gpio
{
public:
    GpioInput(gpio_num_t gpio_num) : Gpio(gpio_num, GPIO_MODE_INPUT)
    {}

    void SetIsrhandler(gpio_isr_t isr_handler, void *args, gpio_int_type_t intr_type = GPIO_INTR_POSEDGE);
};