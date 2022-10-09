#include "GpioOutput.hpp"

void GpioOutput::SetLevel(uint32_t level)
{
    gpio_set_level(m_gpio_num, level);
}