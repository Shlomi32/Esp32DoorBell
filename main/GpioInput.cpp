#include "GpioInput.hpp"


void GpioInput::SetIsrhandler(gpio_isr_t isr_handler, void *args, gpio_int_type_t intr_type)
{
    gpio_set_intr_type(m_gpio_num, intr_type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(m_gpio_num, isr_handler, args);
}