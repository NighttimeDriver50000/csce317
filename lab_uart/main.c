/* library for interacting with AVR interrupts */

#include <avr/interrupt.h>


/* AVR I/O pin definitions */

#include <avr/io.h>


/* standard C library functions */

#include <stdlib.h>


/* string maniuplation functions */

#include <string.h>


/* CPU speed-adjusted delay utilities */

#include <util/delay.h>


int main(void) {


    /* Place your code here. This is just a simple LED blink program. that

     *  will make PC1 blink every 100ms */


    /* Set all pins of port C as an output. We could set just PC1 as an

     * output with DDRC = 0x02 (binary 00000010) if we wanted to */

    DDRC = 0xFF;


    /* loop forever */

    while(1) {


        /* Turn on all pins on port C. We could turn off just PC1

         * using PORTC |= 0x0 */

        PORTC = 0xFF;


        _delay_ms(100);


        /* Turn off all pins on port C. We could turn off just PC1

         * using PORTC &= ~(0x02) */

        PORTC = 0x00;


        _delay_ms(100);

    }

}
