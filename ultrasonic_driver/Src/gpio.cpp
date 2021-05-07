

#include "resource.hpp"

extern "C" void MX_GPIO_Init(void)
{
   quan::stm32::module_enable<quan::stm32::gpioa>();
   quan::stm32::module_enable<quan::stm32::gpiob>();
   quan::stm32::module_enable<quan::stm32::gpioc>();

   quan::stm32::module_enable<led::port_type>();
   quan::stm32::apply<
      led
      , quan::stm32::gpio::mode::output
      , quan::stm32::gpio::otype::push_pull
      , quan::stm32::gpio::pupd::none
      , quan::stm32::gpio::ospeed::slow
      , quan::stm32::gpio::ostate::low
   >();

}
