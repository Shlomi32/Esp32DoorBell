#pragma once

#include "driver/gpio.h"


class Gpio
{
public:
    Gpio(gpio_num_t gpio_num, gpio_mode_t mode) : m_gpio_num{gpio_num}, m_gpio_mode{mode}
    {
        esp_rom_gpio_pad_select_gpio(m_gpio_num);
        gpio_set_direction(m_gpio_num, m_gpio_mode);
    }
    
protected:
    gpio_num_t m_gpio_num;
    gpio_mode_t m_gpio_mode;
};