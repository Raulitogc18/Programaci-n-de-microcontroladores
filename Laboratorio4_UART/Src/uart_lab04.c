#include "uart_lab04.h"
#include "stm32f4xx.h"

#ifndef CPU_HZ
#define CPU_HZ 16000000UL  /* Ajustar si el reloj no es 16 MHz en su proyecto */
#endif

static uint32_t compute_brr(uint32_t baud)
{
    /* Aproximación simple: BRR = fCK / baud (redondeo) */
    return (CPU_HZ + (baud/2U)) / baud;
}

void uart2_init(uint32_t baud, uint8_t enable_rx)
{
    /* 1) Clocks: GPIOA y USART2 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    /* 2) PA2 y PA3 → Alternate Function (MODER=10) */
    GPIOA->MODER &= ~((3U << (2U*2U)) | (3U << (3U*2U)));
    GPIOA->MODER |=  ((2U << (2U*2U)) | (2U << (3U*2U)));

    /* 3) AF7 en PA2 y PA3 (AFRL) */
    GPIOA->AFR[0] &= ~((0xFU << (4U*2U)) | (0xFU << (4U*3U)));
    GPIOA->AFR[0] |=  ((7U   << (4U*2U)) | (7U   << (4U*3U)));

    /* 4) USART2: 8N1 por defecto, baud en BRR */
    USART2->CR1 = 0;
    USART2->BRR = compute_brr(baud);

    /* 5) Habilitar TX y (opcional) RX */
    USART2->CR1 |= USART_CR1_TE;
    if (enable_rx) USART2->CR1 |= USART_CR1_RE;

    /* 6) Enable USART */
    USART2->CR1 |= USART_CR1_UE;
}

void uart2_write_char(char c)
{
    while ((USART2->SR & USART_SR_TXE) == 0) {}
    USART2->DR = (uint8_t)c;
}

void uart2_write_str(const char *s)
{
    if (!s) return;
    while (*s) uart2_write_char(*s++);
}

char uart2_read_char_blocking(void)
{
    while ((USART2->SR & USART_SR_RXNE) == 0) {}
    return (char)(USART2->DR & 0xFF);
}

void uart2_write_u32(uint32_t v)
{
    char buf[11];
    int i = 0;

    if (v == 0) {
        uart2_write_char('0');
        return;
    }
    while (v > 0 && i < 10) {
        buf[i++] = (char)('0' + (v % 10U));
        v /= 10U;
    }
    while (i--) uart2_write_char(buf[i]);
}

/* Redirección mínima para printf (si su toolchain usa __io_putchar) */
int __io_putchar(int ch)
{
    uart2_write_char((char)ch);
    return ch;
}
