#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void setBitByIndex(volatile uint8_t* reg, uint8_t index, uint8_t value) {
    if (value) {
        *reg |= (1 << index);
    } else {
        *reg &= ~(1 << index);
    }
}

void setMultiBitByIndex(volatile uint8_t* reg, uint8_t pairs, ...) {
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
    setMultiBitByIndex(&UCSR0B, 2, TXEN0, 1, RXEN0, 1);

    setMultiBitByIndex(&UCSR0C, 5,
            UCSZ01, 1, UCSZ00, 1, // character size 8
            USBS0, 0, // 1 stop bit
            UPM01, 0, UPM00, 0); // no parity

    fdevopen(uart_putchar, NULL);
    fdevopen(NULL, uart_getchar);

    printf("\n\nUART initialized (%i 8N1)\n", UART_BAUDRATE);
}

volatile uint8_t overflows = 0;

#define DIVIDER (256)
#define INTERVAL ((uint8_t)((F_CPU / DIVIDER) / (1 << 8)))

ISR(TIMER0_OVF_vect) {
    ++overflows;
    if (overflows == INTERVAL / 2) {
        setBitByIndex(&PORTC, PORTC0, 0);
    } else if (overflows >= INTERVAL) {
        setBitByIndex(&PORTC, PORTC0, 1);
        overflows = 0;
    }
}

#define MAX_COMMAND_LENGTH (64)

char command[MAX_COMMAND_LENGTH];
uint8_t commandLength = 0;

uint8_t matchesCommand(char* compare) {
    for (uint8_t i = 0; i < commandLength; ++i) {
        if (compare[i] == '\0' || compare[i] != command[i]) {
            return 0;
        }
    }
    return 1;
}

void printPrompt() {
    printf("\nready> ");
    fflush(stdout);
}

int main(void) {
    uart_init();
    setBitByIndex(&DDRC, DDC0, 1);
    setBitByIndex(&PORTC, PORTC0, 0);
    cli();
    setMultiBitByIndex(&TCCR0B, 3, CS02, 1, CS01, 0, CS00, 0);
    setBitByIndex(&TIMSK0, TOIE0, 1);
    printPrompt();
    int c;
    while (1) {
        c = uart_getchar(stdin);
        if (c == '\r') {
            c = uart_getchar(stdin);
            if (c != '\n') {
                printf("\nInvalid newline: expected \\r\\n or \\n, got \\r\\x%02x", c);
                printPrompt();
                continue;
            }
        }
        if (c == '\r' || c == '\n') {
            if (matchesCommand("on")) {
                overflows = 0;
                TCNT0 = 0;
                setBitByIndex(&PORTC, PORTC0, 1);
                sei();
            } else if (matchesCommand("off")) {
                cli();
                setBitByIndex(&PORTC, PORTC0, 0);
            } else {
                printf("\nUnknown command. Valid commands: on, off");
            }
            printPrompt();
            continue;
        }
        if (commandLength == MAX_COMMAND_LENGTH) {
            printf("\nMaximum command length %d exceeded", MAX_COMMAND_LENGTH);
            printPrompt();
            continue;
        }
        command[commandLength] = c;
        ++commandLength;
    }
}
