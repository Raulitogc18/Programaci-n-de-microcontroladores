#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>

#define GPIOAEN    (1U<<0)
#define UART2EN    (1U<<17)

#define CR1_TE     (1U<<3)
#define CR1_UE     (1U<<13)
#define SR_TXE     (1U<<7)

#define SYS_FREQ   16000000
#define APB1_CLK   SYS_FREQ

#define UART_BAUDRATE  115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

void uart2_tx_init(void);
void uart2_write(int ch);

/* ========= Redirección de printf ========= */
int __io_putchar(int ch)
{
    uart2_write(ch);
    return ch;
}

int main(void)
{
    uart2_tx_init();

    while(1)
    {
        printf("Hello from STM!\r\n"); //toma un texto, lo convierte en caracteres y los envía uno por uno a la UART (mediante __io_putchar) para que aparezcan en el puerto serial.
    }
}

void uart2_tx_init(void)
{
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 to alternate function mode */
    GPIOA->MODER &= ~(1U<<4);
    GPIOA->MODER |=  (1U<<5);

    /* Set PA2 alternate function to AF7 (USART2_TX) */
    GPIOA->AFR[0] |=  (1U<<8);
    GPIOA->AFR[0] |=  (1U<<9);
    GPIOA->AFR[0] |=  (1U<<10);
    GPIOA->AFR[0] &= ~(1U<<11);

    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Enable transmitter */
    USART2->CR1 = CR1_TE;

    /* Enable USART */
    USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
    while(!(USART2->SR & SR_TXE)){}
    USART2->DR = (ch & 0xFF);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
    return ((PeriphClk + (BaudRate/2U)) / BaudRate);
}
