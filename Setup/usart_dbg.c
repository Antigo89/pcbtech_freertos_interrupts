#include "stm32f4xx.h"

void usart1_init(void) {
  /* SET PA9-Tx PA10-Rx */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE9|GPIO_MODER_MODE10);
  GPIOA->MODER |= GPIO_MODER_MODE9_1|GPIO_MODER_MODE10_1;
  GPIOA->AFR[1] |= (7<<GPIO_AFRH_AFSEL9_Pos)|(7<<GPIO_AFRH_AFSEL10_Pos);

  /* Init USART1 module 8N1 115200bod */
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
  // 84MHz / 115200bod / 16 = 45,57  M=45 (0x2D) F=0,57*16=9 (0x09)
  USART1->BRR = 0x02D9;
  USART1->CR1 |= USART_CR1_RXNEIE|USART_CR1_RE;
  USART1->CR1 &= ~(USART_CR1_M|USART_CR1_PCE);
  USART1->CR2 &= ~(USART_CR2_STOP);
  
  NVIC_SetPriority(USART1_IRQn, 14);
  NVIC_EnableIRQ(USART1_IRQn);
  USART1->CR1 |= USART_CR1_UE;
}