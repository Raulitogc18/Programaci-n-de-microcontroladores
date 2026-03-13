#include <stdio.h>
#include "uart.h"

int main(void)
{
    uart2_tx_init();

    while(1)
    {
        printf("Hello from STM32\r\n");
        for(volatile int i = 0; i < 1000000; i++);
    }
}
