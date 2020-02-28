#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MOSI	PORTD5
#define MISO	PORTD4
#define SS		PORTD3
#define SCLK	PORTD2

uint8_t spi_state = 0;
uint8_t spi_data,spi_data_incoming;

void spi_send(uint8_t word) {
	spi_data = word;
	spi_state = 1;
	TCNT1 = 0;
	PORTD = PORTD & ~(1<<SS);
}

/**
 * @brief output one character via UART, on UART0
 *
 * @param c the character
 * @param stream file handle, this is for compatibility
 *
 * @return
 */
int uart_putchar(char c, FILE *stream) {
	while(!(UCSR0A&(1<<UDRE0))){}; //wait while previous byte is completed
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

/**
 * @brief Read one character from UART0, blocking
 *
 * @param stream for compatibility
 *
 * @return
 */
int uart_getchar(FILE *stream) {
	while(!(UCSR0A & (1<<RXC0))){} // wait until data comes
	return UDR0;
}

ISR (SPI_STC_vect) {
	printf ("[slave] received data: 0x%02X\n",SPDR);
}

ISR (TIMER1_COMPA_vect) {
	TCNT1 = 0;
	if (spi_state>0) {
		if ((spi_state & 1)==0) {
			PORTD = PORTD & ~(1<<SCLK);
		} else {
			PORTD = PORTD | (1<<SCLK);
			uint8_t incoming_bit = (PIND >> MISO) & 1;
			spi_data_incoming = (spi_data_incoming << 1) | incoming_bit;

			uint8_t outgoing_bit = spi_data >> 7;
			if (outgoing_bit) {
				PORTD = PORTD | (1<<MOSI);
			} else {
				PORTD = PORTD & ~(1<<MOSI);
			}
			spi_data = spi_data << 1;
		}

		spi_state++;
		if (spi_state==18) {
			spi_state=0;
			PORTD = PORTD | (1<<SS);
			printf ("[master] received: 0x%02X\n",spi_data_incoming);
			spi_data = spi_data_incoming;
		}
	}
}

void init(void) {
	// initialize USART
	UBRR0=(((F_CPU/(UART_BAUDRATE*16UL)))-1); // set baud rate
	UCSR0B|=(1<<TXEN0); //enable TX
	UCSR0B|=(1<<RXEN0); //enable RX

	UCSR0C |= (1 << UCSZ01 ) | (1 << UCSZ00 ) ; // character size of 8
	UCSR0C &= ~(1 << USBS0 ) ; // 1 stop bit
	UCSR0C &= ~( (1 << UPM01 ) | (1 << UPM00 ) ); // disable parity

	// initialize file descriptors
	fdevopen(uart_putchar, NULL);
	fdevopen(NULL, uart_getchar);
	printf("\n\nUART initialized (%u 8N1)\n", UART_BAUDRATE);
	printf("initialization complete.\n\n");	

	DDRD = DDRD | (1<<MOSI) | (1<<SS) | (1<<SCLK);
	DDRD = DDRD & ~(1<<MISO);

	DDRB = (1<<PORTB4);

	PORTD = PORTD | (1<<SCLK);
	PORTD = PORTD | (1<<SS);

	SPCR = SPCR | (1<<SPE) | (1<<SPIE) | (1<<CPOL) | (1<<CPHA);
	SPCR = SPCR & ~(1<<DORD) & ~(1<<MSTR);

	TCCR1B = 1;
	TIMSK1 = 1<<OCIE0A;
	OCR1A = 64;
}

int main(void) {

	init();

	sei();

	SPDR = 0xDE;
	spi_send(0xAD);

	while(1);
}

