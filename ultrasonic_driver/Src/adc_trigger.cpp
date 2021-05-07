
#include "resource.hpp"
#include "stm32l4xx.h"

#include <quan/stm32/get_module_bus_frequency.hpp>
#include <quan/stm32/rcc.hpp>
#include "adc.h"
#include <quan/fixed_quantity/literal.hpp>

/*
   Use DMA to read adc. timer is used to set sampling trigger for a adc read.
   After n samples are read and transferred to mem, average the samples
   and put to result and display.
*/

namespace {

   QUAN_QUANTITY_LITERAL(voltage,V)
   /*
   setup adc channel on PB0 which is ADC_IN15, ARDUINO pin D3
   This allows to add PB1 for differential input (ARDUINO pin D6)

   setup timer6 as trigger so conversions happen on each timer6 overflow
   */

  // typedef quan::mcu::pin<quan::stm32::gpiob,0> analog_in;

 //  typedef quan::stm32::tim6 adc_timer;
  // typedef quan::mcu::pin<quan::stm32::gpiob,3> led;

   /*
   ADC DMA on dma1 channel 1 request 0
   */

   constexpr uint16_t adc_data_length = 50U;
   uint16_t adc_data_array[adc_data_length] = {0U};

   /*
   DMA1   // holds interrupt flags and clear set
   DMA1_Channel1 // holds info about the channel
   DMA1_CSELR  // select the peripheral mapped to the channel
   */

   void adc_dma_setup()
   {
      // enable dma module ahb1 bit 0
      //   quan::stm32::rcc::get()->ahb1enr.bb_setbit<0>();  
      // RCC->AHB1ENR |= (1U << 0U);
      quan::stm32::rcc::get()->ahb1enr.bb_setbit<0>();

      for (uint32_t i = 0; i < 1000; ++i){
         asm volatile ("nop":::);
      }

      //      // select DMA1 Channel1 ADC channel
      DMA1_CSELR->CSELR &= ~(0b1111 << 0U); //C1S[3:0]

      DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;  
      DMA1_Channel1->CMAR = (uint32_t) adc_data_array;  
      DMA1_Channel1->CNDTR = adc_data_length; 

      //  temp &= ~(0b11 << 14U); // (MEM2MEM)  not mem 2 mem
      //  temp &= ~(0b11 << 12U); // (PL[0:1])  priority lowest
      DMA1_Channel1->CCR = 
         (0b01 << 10U)    //(MSIZE[1:0]) memsize 16 bit
         | (0b01 << 8U)   // (PSIZE[1:0]) perip size 16 bit
         | (1U << 7U)     // (MINC) incr memory
         | (1U << 5U);     // (CIRC) circular mode
      //      temp &= ~(1U << 6U);    // (PINC) dont inc periph
      //      temp &= ~(1U << 4U);    // (DIR) read from peripheral

      //      // CMSIS DMA irq call
      NVIC_SetPriority(DMA1_Channel1_IRQn,15); // low priority 
      NVIC_EnableIRQ(DMA1_Channel1_IRQn);

      // clear flags and enable tc irq
      DMA1->IFCR = (1 << 0U);
      DMA1_Channel1->CCR |= (1U << 1U);    // (TCIE) 
   }

   void adc_timer_setup()
   {
      quan::stm32::module_enable<adc_timer>();

      auto constexpr bus_freq = quan::stm32::get_module_bus_frequency<adc_timer>();
      auto constexpr target_freq = 10000U; // 10 kHz
      auto constexpr prescaler_value = bus_freq / target_freq;

      adc_timer::get()->psc = prescaler_value - 1U;
      adc_timer::get()->cnt = 0U;
      adc_timer::get()->arr = target_freq/(10U * adc_data_length) - 1U;  // 10 HZ DMA transfer complete
      adc_timer::get()->sr = 0U;

      // set up  update event as TRGO output for triggering an adc conversion sequence
      adc_timer::get()->cr2 |= (0b010 << 4U); //(MMS[6:4]) 
      // N.B that adc must be enabled
      // dont start the timer
      // quan::stm32::enable<adc_timer>();
   }

   bool new_conversion = false;
   uint32_t latest_adc_voltage = 0U;
} // ~namespace


void adc_setup()
{
   // setup the pin
   quan::stm32::module_enable<analog_in::port_type>();
   quan::stm32::apply<
      analog_in
     ,quan::stm32::gpio::mode::analog
     ,quan::stm32::gpio::pupd::none
   >();
   // set adc clock to sysclk
   // ADC clock source is set in rcc.ccipr bits 29:28
   // ref man 6.4.27
   //    00 no adc clock
   //    01 PLLSAI1 "R " clock 
   //    10 reserved
   // -> 11 system clock 
   quan::stm32::rcc::get()->ccipr |= (0b11 << 28U) ; // (ADCSEL[1:0])
   // ref_man 6.2.12
   // enable the adc module rcc.ahb2enr bit 13
   quan::stm32::rcc::get()->ahb2enr.bb_setbit<13>(); // (ADCEN)

   // set input clock to AHB div by 4 to give a 20 MHz ADC clock
   ADC1_COMMON->CCR |= (0b11 << 16U); // (CKMODE[1:0] ) <-- HCLK/4

   // take adc out of deep powerdown mode
   ADC1->CR &= ~(1U << 29U); // (DEEPPWD)
   // enable the adc voltage regulator
   ADC1->CR |= (1U << 28U); // (ADVREGEN)
   // wait for T ADCVREG_STUP which is 20 usec according to datasheet
   for ( uint32_t i = 0U; i < 80U * 20U; ++i){
      asm volatile("nop":::);
   }
   // for now use single ended mode
   // Therefore ADCALDIF = 0 for calibration, but needs to be 1 for differential mode
   // Do calibration
   ADC1->CR |= (1U << 31U); // (ADCAL)

   // setup timer while waiting for calibration
   adc_timer_setup();

   // wait for calibration to complete
   while ( (ADC1->CR & (1U << 31U)) == 0U ){ // (ADCAL)
      asm volatile("nop":::);
   }

   // Now enable ADC
   ADC1->CR |= ( 1U << 0U);// (ADEN)

   // n.b I think that ADEN must be on before calling dma_setup
   adc_dma_setup();
    // wait for ADCready
   while ( (ADC1->ISR & (1U << 0U)) == 0U ){ // (ADRDY)
      asm volatile("nop":::);
   }
   // Timing ref_man 16.4.16

   // sample time TSmpl is controlled by SMPR1 and SMPR2 regs
   // for 12 bit conversion
   // min sample time is 0.03125 usec, max is 8.00625 usec
   // at 20 MHz clock tclk = 0.05 usec ncycles for min sampling time = 1
   // minimum avail is 2.5 adc clks == 0.125 usec
   // so default in smprx is ok
   
   // select adc channel ADC_IN15. Put in first slot in SQR
   ADC1->SQR1 |= (15U << 6U); // (SQ1[4:0])
   // N.B for more than 1 channel, set n_channels -1 in  SQR1 L reg

   // enable cmsis irq machinery
   NVIC_SetPriority(ADC1_IRQn,15); // low priority 
   NVIC_EnableIRQ(ADC1_IRQn);

   // select trigger rising edge
   ADC1->CFGR |= (0b01 << 10U); // (EXTEN[1:0])
   // select EXT13 TIM6 overflow as trigger
   ADC1->CFGR |= (13U << 6U); // (EXTSEL[3:0])

   // if not using DMA enable eocie
   //  ADC1->IER |= (1U << 2U); // (EOCIE)
   // for dma, no irq on EOC
   // ADC1->IER &= ~(1U << 2U); // (EOCIE)

   DMA1->IFCR = (1 << 0U); // clear dma flags
   DMA1_Channel1->CCR |= (1U << 0U); // (EN)
   ADC1->CFGR |= (1U << 0U) ; // (DMAEN)
   // start the ADC
   ADC1->CR |= (1U << 2U);// (ADSTART)
   // start the adc_timer which provides the trigger
   quan::stm32::enable<adc_timer>();

   //quan::stm32::clear<led>();
}

// not used if dma used
extern "C" __attribute__ ((interrupt ("IRQ")))
void ADC1_IRQHandler()
{
   // read the voltage which clears the flag
   latest_adc_voltage = ADC1->DR;
   // clear the adc_timer  UIF flag 
   adc_timer::get()->sr = 0U; 
   // signal new conversion
   new_conversion = true;
   //quan::stm32::complement<led>();
}

extern "C" __attribute__ ((interrupt ("IRQ")))
void DMA1_Channel1_IRQHandler()
{
   // disable DMA
 //  if ( DMA1->ISR  & ( 1U << 1U)){
//    quan::stm32::complement<led>();
  // }
   //clear flags
   DMA1->IFCR = (1 << 0U);

   // calc average of values acquired
   uint32_t sum = 0;
   for ( uint32_t i = 0; i < adc_data_length; ++i){
    sum += adc_data_array[i];
   }
   latest_adc_voltage = sum/ adc_data_length + (((sum % adc_data_length) >= adc_data_length/2)?1:0);

   // signal new conversion
   new_conversion = true;

   // restart adc
   ADC1->CR |= (1U << 2U);// (ADSTART)
}

quan::voltage_<float>::V read_adc()
{
   return quan::voltage_<float>::V{latest_adc_voltage * 3.3_V / 4096};
}

bool have_new_adc_conversion()
{
   return new_conversion;
}

void clear_new_adc_conversion()
{
   new_conversion = false;
}

/*
setup for basic adc with manual read
*/

void adc_simple_setup()
{
   // setup the pin
   quan::stm32::module_enable<analog_in::port_type>();
   quan::stm32::apply<
      analog_in
     ,quan::stm32::gpio::mode::analog
     ,quan::stm32::gpio::pupd::none
   >();
   // set adc clock to sysclk
   // ADC clock source is set in rcc.ccipr bits 29:28
   // ref man 6.4.27
   //    00 no adc clock
   //    01 PLLSAI1 "R " clock 
   //    10 reserved
   // -> 11 system clock 
   quan::stm32::rcc::get()->ccipr |= (0b11 << 28U) ; // (ADCSEL[1:0])
   // ref_man 6.2.12
   // enable the adc module rcc.ahb2enr bit 13
   quan::stm32::rcc::get()->ahb2enr.bb_setbit<13>(); // (ADCEN)

   // set input clock to AHB div by 4 to give a 20 MHz ADC clock
   ADC1_COMMON->CCR |= (0b11 << 16U); // (CKMODE[1:0] ) <-- HCLK/4

   // take adc out of deep powerdown mode
   ADC1->CR &= ~(1U << 29U); // (DEEPPWD)
   // enable the adc voltage regulator
   ADC1->CR |= (1U << 28U); // (ADVREGEN)
   // wait for T ADCVREG_STUP which is 20 usec according to datasheet
   for ( uint32_t i = 0U; i < 80U * 20U; ++i){
      asm volatile("nop":::);
   }
   // for now use single ended mode
   // Therefore ADCALDIF = 0 for calibration, but needs to be 1 for differential mode
   // Do calibration
   ADC1->CR |= (1U << 31U); // (ADCAL)
   // wait for calibration to complete
   while ( (ADC1->CR & (1U << 31U)) == 0U ){ // (ADCAL)
       asm volatile("nop":::);
   }

   // Now enable ADC
   ADC1->CR |= ( 1U << 0U);// (ADEN)
    // wait for ADCready
   while ( (ADC1->ISR & (1U << 0U)) == 0U ){ // (ADRDY)
       asm volatile("nop":::);
   }
   // Timing ref_man 16.4.16

   // sample time TSmpl is controlled by SMPR1 and SMPR2 regs
   // for 12 bit conversion
   // min sample time is 0.03125 usec, max is 8.00625 usec
   // at 20 MHz clock tclk = 0.05 usec ncycles for min sampling time = 1
   // minimum avail is 2.5 adc clks == 0.125 usec
   // so default in smprx is ok
   
   // select adc channel ADC_IN15. Put in first slot in SQR
   ADC1->SQR1 |= (15U << 6U); // (SQ1[4:0])
   // N.B for more than 1 channel, set n_channels -1 in  SQR1 L reg
}

// basic manual while you wait adc conv
quan::voltage_<float>::V do_simple_adc_conversion()
{
   // start conv ADSTART =1
   ADC1->CR |= (1U << 2U);// (ADSTART)
   // wait for conv to finish EOC= 1
   while ( (ADC1->ISR & (1U << 2U)) == 0U){
      asm volatile("nop":::);
   }
   // read result;
   return (ADC1->DR * 3.3_V) / 4096;
}



