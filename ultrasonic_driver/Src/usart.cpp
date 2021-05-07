/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <quan/stm32/irq_handler.hpp>

#include <quan/stm32/usart/irq_handler.hpp>

#define QUAN_STM32_WANT_USART2_IRQ_HANDLER

void MX_USART2_UART_Init(void)
{
   serial::port::init();
   serial::port::set_baudrate<115200,true>();
}

template <> __attribute__ ((interrupt ("IRQ"))) 
void quan::stm32::irq_handler<serial::port::usart_type>()
{
   quan::stm32::usart::irq_handler<serial::port>();
}

#if defined(QUAN_STM32_WANT_USART1_IRQ_HANDLER)
// make USART1_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::usart1 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_5usart6moduleILm1073821696EEEEEvv")))
void USART1_IRQHandler();
#endif

#if defined (QUAN_STM32_WANT_USART2_IRQ_HANDLER)
//make USART2_IRQHandler alias for quan::stm32::irq_handler< quan::stm32::usart2 >()
extern "C" __attribute__ ((interrupt ("IRQ"), alias ("_ZN4quan5stm3211irq_handlerINS0_5usart6moduleILm1073759232EEEEEvv")))
void USART2_IRQHandler();
#endif






