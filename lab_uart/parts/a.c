#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

static inline void setBitByIndex(volatile uint8_t* reg, uint8_t index, uint8_t value) {
    if (value) {
        *reg |= (1 << index);
    } else {
        *reg &= ~(1 << index);
    }
}

int main(void) {
    setBitByIndex(&DDRC, DDC0, 1);
    while (1) {
        setBitByIndex(&PORTC, PORTC0, 0);
        _delay_ms(500);
        setBitByIndex(&PORTC, PORTC0, 1);
        _delay_ms(500);
    }
}
