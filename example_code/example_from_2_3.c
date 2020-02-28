#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* library for interacting with AVR interrupts */
#include <avr/interrupt.h>

/**
 * @brief output one character via UART, on UART0
 *
 * @param c the character
 * @param stream file handle, this is for compatibility
 *
 * @return
 */
int uart_putchar(char c, FILE *stream) {
    /* wait for the transmitter to become ready */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    /* because it is assumed by most serial consoles, we automatically
     * rewrite plain LF endings to CRLF line endings */
    if (c == '\n') {
        uart_putchar('\r', stream);
    }

    /* wait for the transmitter to become ready, since we might have just
     * transmitted a CR */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    /* transmit the character */
    UDR0 = c;

    /* signal success */
    return 0;
}

/**
 * @brief Read one character from UART0, blocking
 *
 * @param stream for compatibility
 *
 * @return the character that was read
 */
int uart_getchar(FILE *stream) {

    /* wait for the data to arrive */
    loop_until_bit_is_set(UCSR0A, RXC0);

    /* return the received data */
    return UDR0;

}

/**
 * @brief Initialize UART for use with stdio
 */
void uart_init(void) {
    /* initialize USART0 */
    UBRR0=(((F_CPU/(UART_BAUDRATE*16UL)))-1); // set baud rate
    UCSR0B|=(1<<TXEN0); //enable TX
    UCSR0B|=(1<<RXEN0); //enable RX

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // character size of 8
    UCSR0C &= ~(1 << USBS0); // 1 stop bit
    UCSR0C &= ~( (1 << UPM01) | (1 << UPM00) ); // disable parity

    /* initialize file descriptors, notice the functions we wrote earlier
     * are used as callbacks here */
    fdevopen(uart_putchar, NULL);
    fdevopen(NULL, uart_getchar);
    printf("\n\nUART initialized (%i 8N1)\n", UART_BAUDRATE);
}

volatile uint16_t overflows = 0;
volatile uint8_t foo;
volatile uint8_t pressed=0;

ISR(TIMER1_OVF_vect) {
    overflows++;
    //cli();
}

ISR(USART_RX_vect) {
    printf ("rx interrupt\n");
    cli();
    foo = UDR0;
    pressed=1;
}

int main(void) {
	uart_init();

    /* Place your code here. This is just a simple LED blink program. that
     *  will make PC1 blink every 100ms */

    /* Set all pins of port C as an output. We could set just PC1 as an
     * output with DDRC = 0x02 (binary 00000010) if we wanted to */
    DDRC = 0xFF;

    cli();
    TCCR1B = TCCR1B | (1<<CS10);
    TIMSK1 = TIMSK1 | (1<<TOIE1);
    UCSR0B = UCSR0B | (1<<RXCIE0);
    TCNT1 = 0;
    sei();

    /* loop forever */
    while(1) {

	uint16_t waits = rand()&((1<<10)-1);
	uint16_t count = 0;
	TCNT1 = 0;
	sei();
	while (overflows < waits);
	cli();
	PORTC = 0xFF;

	pressed = 0;
	overflows = 0;
	TCNT1 = 0;

	sei();
	while (pressed==0);
	cli();
	PORTC = 0;
	printf ("your response time is %lu cycles\n",((uint32_t)overflows << 16) | TCNT1);
    }
}

