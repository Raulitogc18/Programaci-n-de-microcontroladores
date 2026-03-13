#include "uart.h"
#include "stm32f4xx.h"

#define GPIOAEN    (1U<<0)
#define UART2EN    (1U<<17)

/* Control register bits */
#define CR1_TE     (1U<<3)
#define CR1_RE     (1U<<2)
#define CR1_UE     (1U<<13)

/* Status register bits */
#define SR_TXE     (1U<<7)
#define SR_RXNE    (1U<<5)

#define SYS_FREQ        16000000UL
#define APB1_CLK        SYS_FREQ
#define UART_BAUDRATE   115200UL

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

/* Redirect printf */
int __io_putchar(int ch)
{
    uart2_write(ch);
    return ch;
}

/* ============================= */
/* RX + TX INIT */
/* ============================= */
void uart2_rxtx_init(void)
{
    /* Enable clock access to GPIOA */
    RCC->AHB1ENR |= GPIOAEN;

    /* ---- PA2 = TX ---- */
    GPIOA->MODER &= ~(3U << (2*2));      // clear bits
    GPIOA->MODER |=  (2U << (2*2));      // alternate function

    GPIOA->AFR[0] &= ~(0xF << (4*2));    // clear
    GPIOA->AFR[0] |=  (7U  << (4*2));    // AF7

    /* ---- PA3 = RX ---- */
    GPIOA->MODER &= ~(3U << (3*2));
    GPIOA->MODER |=  (2U << (3*2));

    GPIOA->AFR[0] &= ~(0xF << (4*3));
    GPIOA->AFR[0] |=  (7U  << (4*3));

    /* Enable clock access to USART2 */
    RCC->APB1ENR |= UART2EN;

    /* Configure baudrate */
    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    /* Enable TX and RX */
    USART2->CR1 = CR1_TE | CR1_RE;

    /* Enable USART */
    USART2->CR1 |= CR1_UE;
}

/* ============================= */
/* TX ONLY INIT */
/* ============================= */
void uart2_tx_init(void)
{
    RCC->AHB1ENR |= GPIOAEN;

    GPIOA->MODER &= ~(3U << (2*2));
    GPIOA->MODER |=  (2U << (2*2));

    GPIOA->AFR[0] &= ~(0xF << (4*2));
    GPIOA->AFR[0] |=  (7U  << (4*2));

    RCC->APB1ENR |= UART2EN;

    uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

    USART2->CR1 = CR1_TE;
    USART2->CR1 |= CR1_UE;
}

/* ============================= */
/* READ */
/* ============================= */
char uart2_read(void)
{
    while(!(USART2->SR & SR_RXNE)){}
    return (char)(USART2->DR & 0xFF);
}

/* ============================= */
/* WRITE */
/* ============================= */
void uart2_write(int ch)
{
    while(!(USART2->SR & SR_TXE)){}
    USART2->DR = (ch & 0xFF);
}

/* ============================= */
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
    USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
    return (PeriphClk + (BaudRate/2U)) / BaudRate;
}
