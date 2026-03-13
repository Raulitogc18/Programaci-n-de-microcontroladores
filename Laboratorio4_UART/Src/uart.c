#include "uart.h"
#include "stm32f4xx.h"

#define GPIOAEN    (1U << 0)
#define USART2EN   (1U << 17)

#define CR1_TE     (1U << 3)
#define CR1_UE     (1U << 13)

#define SR_TXE     (1U << 7)

#define UART_BAUD   115200UL
#define PCLK1_FREQ  16000000UL   /* TU micro está en 16 MHz */

void uart2_tx_init(void)
{
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* Set PA2 to Alternate Function mode */
    GPIOA->MODER &= ~(3U << (2 * 2));
    GPIOA->MODER |=  (2U << (2 * 2));

    /* Set PA2 to AF7 (USART2_TX) */
    GPIOA->AFR[0] &= ~(0xF << (4 * 2));
    GPIOA->AFR[0] |=  (7U << (4 * 2));

    /* Enable clock access to USART2 */
    RCC->APB1ENR |= USART2EN;

    /* Configure baudrate */
    USART2->BRR = (PCLK1_FREQ + (UART_BAUD/2U)) / UART_BAUD;

    /* Enable transmitter */
    USART2->CR1 = CR1_TE;

    /* Enable USART2 */
    USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
    while (!(USART2->SR & SR_TXE)) {}
    USART2->DR = (ch & 0xFF);
}

int __io_putchar(int ch)
{
    uart2_write(ch);
    return ch;
}
