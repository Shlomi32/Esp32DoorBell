#pragma once

#include "Gpio.hpp"

class GpioOutput : public Gpio
{
public:
        GpioOutput(gpio_num_t gpio_num) : Gpio(gpio_num, GPIO_MODE_OUTPUT)
        {}

        void SetLevel(uint32_t level);
};