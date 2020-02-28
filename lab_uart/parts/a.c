include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

/*static inline void creates a function which will allow the compiler to do inling which
  typically requires optimization to be enabled.
  exp -
  static inline void swap(int *m, int *n)
   {
    int tmp = *m;
    *m = *n;
    *n = tmp;
   }
   along with this we have uint8_t which a byte, shorthand for unsigned integer of length
   8 bits.

*/
static inline void setBitByIndex(volatile uint8_t* reg, uint8_t index, uint8_t value) {
    if (value) {
        *reg |= (1 << index);
    } else {
        *reg &= ~(1 << index);
    }
}

//main method
int main(void) {

	//function call for setBit by index where register is the DataDirection Register DDRD
	//the index is 
	setBitByIndex(&DDRC, DDC0, 1);
	//while loop for moving though the 
    while (1) {
        setBitByIndex(&PORTC, PORTC0, 0);
        _delay_ms(500);
        setBitByIndex(&PORTC, PORTC0, 1);
        _delay_ms(500);
    }
}
