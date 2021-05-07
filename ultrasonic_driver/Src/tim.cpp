

#include "resource.hpp"
#include <quan/stm32/get_module_bus_frequency.hpp>
#include <quan/stm32/tim/detail/get_irq_number.hpp>
#include <quan/stm32/irq_handler.hpp>

#include "tim.h"

namespace {

// required to alias the correct c irq function to the tim1 irq handler
#define QUAN_STM32_WANT_TIM15_IRQ_HANDLER
//#define QUAN_STM32_WANT_TIM1_IRQ_HANDLER
}

void MX_TIM_Init(void)
{
   
   quan::stm32::module_enable<dummy_timer>();
   
   auto constexpr bus_freq = quan::stm32::get_module_bus_frequency<dummy_timer>();
   auto constexpr target_freq = 10000U;
   auto constexpr prescaler_value = bus_freq / target_freq;

   dummy_timer::get()->psc = prescaler_value - 1U;
   dummy_timer::get()->cnt = 0U;
   dummy_timer::get()->arr = target_freq/2 - 1U;  // 0.5 HZ overflow
   dummy_timer::get()->sr = 0U;

   // enable update irq
   static constexpr IRQn_Type irq_number = quan::stm32::tim::detail::get_irq_number<dummy_timer>::value;
   NVIC_SetPriority(irq_number,15); // low priority 
   NVIC_EnableIRQ(irq_number);

   dummy_timer::get()->dier.bb_setbit<0>(); // (UIE)
  
   quan::stm32::enable<dummy_timer>();

}
/*
   TODO 
   where 2 timers use same irq say update<timer1>, timer15
   alias the function to quan::stm32::irq_handler<quan::meta::type_sequence< update<timer1>, timer15> >();
*/

template <> __attribute__ ((interrupt ("IRQ")))
void quan::stm32::irq_handler<dummy_timer>()
{
   dummy_timer::get()->sr = 0U;
  // quan::stm32::complement<led>();
}

//#if defined QUAN_STM32_WANT_TIM1_IRQ_HANDLER
//// make TIM1_UP_TIM16_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim1 >()
//extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073818624EEEEEvv")))
//void TIM1_UP_TIM16_IRQHandler();
//#endif

#if defined QUAN_STM32_WANT_TIM2_IRQ_HANDLER
// make TIM2_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim2 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073741824EEEEEvv")))
void TIM2_IRQHandler();
#endif

#if defined QUAN_STM32_WANT_TIM6_IRQ_HANDLER
// make TIM6_DAC_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim6 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073745920EEEEEvv")))
void TIM6_DAC_IRQHandler();
#endif

#if defined QUAN_STM32_WANT_TIM7_IRQ_HANDLER
// make TIM7_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim7 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073746944EEEEEvv")))
void TIM7_IRQHandler();
#endif

#if defined QUAN_STM32_WANT_TIM15_IRQ_HANDLER
// make TIM1_BRK_TIM15_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim15 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073823744EEEEEvv")))
void TIM1_BRK_TIM15_IRQHandler();
#endif

#if defined QUAN_STM32_WANT_TIM16_IRQ_HANDLER
// make TIM1_UP_TIM16_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::tim16 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_3tim6moduleILm1073824768EEEEEvv")))
void TIM1_UP_TIM16_IRQHandler();
#endif
