#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lis3dh.h"

/////// BIT MANIPULATION //////////////////////////////////////////////////////

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

/////// UART I/O //////////////////////////////////////////////////////////////

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

/////// SPI I/O ///////////////////////////////////////////////////////////////

#define SS   PORTB2
#define MOSI PORTB3
#define MISO PORTB4
#define SCLK PORTB5

void spi_init(void) {
    setMultiBitByIndex(&SPCR, 8,
            SPIE, 0, // no SPI interrupt
            SPE, 1, // enable SPI
            DORD, 0, // MSB first
            MSTR, 1, // master mode
            CPOL, 0, // leading rising, trailing falling
            CPHA, 0, // leading sample, trailing setup
            SPR1, 1, SPR0, 1); // 128 clock divider
    setMultiBitByIndex(&DDRB, 4, DDB2, 1, DDB3, 1, DDB4, 0, DDB5, 1);
    setBitByIndex(&PORTB, SS, 1);
}

volatile uint8_t spi_batch_count = 0;

// NOTE: These have no overflow/underflow protections. Use at own risk.
void push_spi_batch() {
    ++spi_batch_count;
    setBitByIndex(&PORTB, SS, 0);
}
void pop_spi_batch() {
    --spi_batch_count;
    if (spi_batch_count == 0) {
        setBitByIndex(&PORTB, SS, 1);
    }
}

uint8_t spi_transfer(uint8_t value) {
    push_spi_batch();
    SPDR = value;
    loop_until_bit_is_set(SPSR, 7);
    uint8_t ret = SPDR;
    pop_spi_batch();
    return ret;
}

/////// LIS3DH I/O ////////////////////////////////////////////////////////////

uint8_t lis3dh_read(uint8_t reg) {
    push_spi_batch();
    spi_transfer(reg | 0x80);
    uint8_t ret = spi_transfer(0xff);
    pop_spi_batch();
    return ret;
}

void lis3dh_write(uint8_t reg, uint8_t value) {
    push_spi_batch();
    spi_transfer(reg & ~0x80);
    spi_transfer(value);
    pop_spi_batch();
}

int16_t bytes2short(uint8_t lo, uint8_t hi) {
    uint16_t hi16 = hi;
    uint16_t raw = (hi16 << 8) | lo;
    return *(int16_t*)(&raw);
}

volatile int16_t x = 0;
volatile int16_t y = 0;
volatile int16_t z = 0;

void lis3dh_update_xyz() {
    uint8_t xl = lis3dh_read(OUT_X_L);
    uint8_t xh = lis3dh_read(OUT_X_H);
    uint8_t yl = lis3dh_read(OUT_Y_L);
    uint8_t yh = lis3dh_read(OUT_Y_H);
    uint8_t zl = lis3dh_read(OUT_Z_L);
    uint8_t zh = lis3dh_read(OUT_Z_H);
    x = bytes2short(xl, xh);
    y = bytes2short(yl, yh);
    z = bytes2short(zl, zh);
}

//#define POLLING

void lis3dh_setup(void) {
    cli();
    lis3dh_write(CTRL_REG1, 0x27); // 25Hz, normal mode, X/Y/Z all enabled
    lis3dh_write(CTRL_REG5, 0x08); // No FIFO, latch interrupts
    lis3dh_write(FIFO_CTRL_REG, 0x00); // No FIFO, so Bypass
    lis3dh_write(CTRL_REG3, 0x10); // Enable DRDY1 interrupt on INT1
    lis3dh_write(CTRL_REG6, 0x02); // Active low INT1
    setMultiBitByIndex(&EICRA, 2, ISC01, 0, ISC00, 0);
#if !defined(POLLING)
    setBitByIndex(&EIMSK, INT0, 1);
#endif
    lis3dh_update_xyz();
    sei();
}

/////// LIS3DH DATA READY INTERRUPT HANDLER ///////////////////////////////////

volatile uint8_t int1_src = 0;
volatile uint8_t status_reg = 0;

ISR(INT0_vect) {
    lis3dh_update_xyz();
    int1_src = lis3dh_read(INT1_SOURCE);
    status_reg = lis3dh_read(STATUS_REG2);
}

/////// 1 HZ PRINTING /////////////////////////////////////////////////////////

volatile uint8_t overflows = 0;

#define DIVIDER (256)
#define INTERVAL ((uint8_t)((F_CPU / DIVIDER) / (1 << 8)))

ISR(TIMER0_OVF_vect) {
    ++overflows;
    if (overflows >= INTERVAL) {
#if defined(POLLING)
        lis3dh_update_xyz();
#endif
        printf("XYZ: (%+d, %+d, %+d)\n", x, y, z);
#if !defined(POLLING)
        //printf("INT1_SOURCE: 0x%02x\n", int1_src);
        //printf("STATUS_REG2: 0x%02x\n", status_reg);
#endif
        overflows = 0;
    }
}

/////// MAIN //////////////////////////////////////////////////////////////////

int main(void) {
    cli();
    uart_init();
    setBitByIndex(&DDRC, DDC0, 1);
    setBitByIndex(&PORTC, PORTC0, 0);
    setMultiBitByIndex(&TCCR0B, 3, CS02, 1, CS01, 0, CS00, 0);
    setBitByIndex(&TIMSK0, TOIE0, 1);
    spi_init();
    lis3dh_setup();
    while (1) { }
}
