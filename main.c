/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

Задание
Реализовать систему управления с терминалом доступа. Пользователь должен иметь возможность
управлять включением и выключением пользовательских светодиодов демо-платы с помощью отправки
команд по последовательному интерфейсу. Основные условия:
1. Количество управляемых светодиодов определяется разработчиком, но их количество должно быть
не менее двух.
2. Сценарий поведения системы на примере трёх светодиодов:
2.1. Исходное состояние системы – диоды выключены.
2.2. При отправке символа «1» светодиод с условным именем LED1 должен переключать своё
состояние (должен зажечься, если он потушен и наоборот – потухнуть если он светился);
2.3. При отправке символа «2» светодиод LED2 должен переключать своё состояние;
2.4. При отправке символа «3» светодиод LED3 должен переключать своё состояние;
3. Выполнить сборку, запуск и тестирование проекта на отладочной плате.
Критерии готовности (Definition of Done)
1. Проект собирается и загружается в отладочную плату без ошибок и предупреждений.
2. Начальные условия при включении системы соблюдаются.
3. На контроллере реализовано получение команд по последовательному порту и их последующая
обработка согласно описанному сценарию поведения системы.
4. В решении используется прерывание по приёму данных по UART и один из механизмов отложенной
обработки прерываний FreeRTOS (отдельными задачами обработки полученных данных или через
задачу-демон программных таймеров). Выбор механизма отложенной обработки прерываний
определяется разработчиком самостоятельно.


USING STM32F407VET6 board:

SWD interface
PE13 output LED1
PE14 output LED2
PE15 output LED3
PA9 & PA10 - USART1
*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "gpio.h"
#include "RCC_Init.h"
#include "usart_dbg.h"


#include <stdio.h>
#include <stdlib.h>


/************************global values********************************/
#define QUEUE_REC_NUM (10) //record number in queue

/* Queue handler */
QueueHandle_t myQueue;
/****************************func************************************/

/* Read queue */
void vTaskControl(void * pvParameters){
  char receiveMsg;
  while(1){
    xQueueReceive(myQueue, &receiveMsg, portMAX_DELAY);
    int8_t receive_numer = receiveMsg - '0';
    switch (receive_numer) {
      case 1:
        if(GPIOE->ODR & GPIO_ODR_OD13){
          GPIOE->BSRR |= GPIO_BSRR_BR13;
        }else{
          GPIOE->BSRR |= GPIO_BSRR_BS13;
        }
        break;
      case 2:
        if(GPIOE->ODR & GPIO_ODR_OD14){
          GPIOE->BSRR |= GPIO_BSRR_BR14;
        }else{
          GPIOE->BSRR |= GPIO_BSRR_BS14;
        }
        break;
      case 3:
        if(GPIOE->ODR & GPIO_ODR_OD15){
          GPIOE->BSRR |= GPIO_BSRR_BR15;
        }else{
          GPIOE->BSRR |= GPIO_BSRR_BS15;
        }
        break;
      default:
        break;
    }
    
  }
  vTaskDelete(NULL);
}
/****************************main************************************/

 int main(void) {
  RCC_Init();
  GPIO_init();
  usart1_init();
  printf("--- System startup ---\n");
  
  myQueue = xQueueCreate(QUEUE_REC_NUM, sizeof(char));

  xTaskCreate(vTaskControl, "vTaskControl", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskStartScheduler();
  while(1){}
}
/**************************Interrupt func****************************/

void USART1_IRQHandler(void){
  if(USART1->SR & USART_SR_RXNE){
    char receive = (uint16_t)USART1->DR & (uint16_t)0x01FF;
    BaseType_t taskWoken;
    xQueueSendFromISR(myQueue, &receive, &taskWoken);
    portYIELD_FROM_ISR(taskWoken);
  }
  NVIC_ClearPendingIRQ(USART1_IRQn);
}

/*************************** End of file ****************************/
