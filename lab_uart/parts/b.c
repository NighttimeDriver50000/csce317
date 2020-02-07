#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <avr/io.h>
#include <util/delay.h>

static inline void setBitByIndex(volatile uint8_t* reg, uint8_t index, uint8_t value) {
    if (value) {
        *reg |= (1 << index);
    } else {
        *reg &= ~(1 << index);
    }
}

static inline void setBitsByIndex(volatile uint8_t* reg, uint8_t pairs, ...) {
    va_list args;
    va_start(args, pairs);
    for (uint8_t i = 0; i < pairs; ++i) {
        uint8_t index = (uint8_t)va_arg(args, int);
        uint8_t value = (uint8_t)va_arg(args, int);
        setBitByIndex(reg, index, value);
    }
    va_end(args);
}

int uart_putchar(char c, FILE *stream) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

int uart_getchar(FILE *stream) {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

void uart_init(void) {
    UBRR0 = (F_CPU / (UART_BAUDRATE * 16UL)) - 1;
    setBitsByIndex(&UCSR0B, 2, TXEN0, 1, RXEN0, 1);

    setBitsByIndex(&UCSR0C, 5,
            UCSZ01, 1, UCSZ00, 1, // character size 8
            USBS0, 0, // 1 stop bit
            UPM01, 0, UPM00, 0); // no parity

    fdevopen(uart_putchar, NULL);
    fdevopen(NULL, uart_getchar);

    printf("\n\nUART initialized (%i 8N1)\n", UART_BAUDRATE);
}

int main(void) {
    uart_init();
    printf("Hello World!\n");
}
