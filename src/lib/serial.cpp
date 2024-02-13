/**
 * @file serial.c
 * 
 * From https://github.com/tuupola/avr_demo/tree/master/blog/simple_usart
 */

#include <lib/serial.h>

int uart_putchar(char c, FILE *stream) {
    if(c == '\n')
        uart_putchar('\r', stream);
    
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

int uart_getchar(FILE *stream)
{
    loop_until_bit_is_set(UCSRA, RXC); /* Wait until data exists. */
    return UDR;
}

void uart_init()
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

#if USE_2X
    UCSRA |= _BV(U2X);
#else
    UCSRA &= ~(_BV(U2X));
#endif

    UCSRC = _BV(UCSZ1) | _BV(UCSZ0); /* 8-bit data */
    UCSRB = _BV(RXEN) | _BV(TXEN);   /* Enable RX and TX */

    stdout = fdevopen(uart_putchar, NULL);
    stdin  = fdevopen(NULL, uart_getchar);
}
