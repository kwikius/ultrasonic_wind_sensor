

#include "main.h"
#include "stm32l4xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "adc.h"

void SystemClock_Config(void);

void setup()
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();

  MX_TIM_Init();

 // adc_setup();

}