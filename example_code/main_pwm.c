#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define	SETBIT(PORT,pin,val)	if (val) PORT|=(1<<pin); else PORT&=~(1<<pin);

uint8_t LED_map[7]={DDD0,DDD1,DDD2,DDD3,DDD4,DDD5,DDD7};

int main(void) {
	DDRD = 0xFF;

	// set up PWM
	TCCR0A = (1<<COM0A1);
	TCCR0A |= (1<<WGM01) | (1<<WGM00);
	TCCR0B = (1<<CS00);

	OCR0A = 127;
/*
	// DEMO 1:  all LEDs "breathe"
	uint8_t updown=0;
	while (1) {
		if (updown==0) OCR0A++; else OCR0A--;

		if (updown==0 && OCR0A==255) updown=1;
		if (updown==1 && OCR0A==0) updown=0;

		_delay_ms(2);
	}
*/

/*
	// DEMO 2:  a randomly-changing subset of LEDs "breathe"
	uint8_t updown=0;
	while (1) {
		if (updown==0) OCR0A++; else OCR0A--;
		if (updown==0 && OCR0A==255) updown=1;
		if (updown==1 && OCR0A==0) updown=0;
		if (rand()<64)
		for (uint8_t i=0;i<7;i++) SETBIT(PORTD,LED_map[i],rand()<16384?0:1)
	
		_delay_ms(2);
	}
*/

	// DEMO 3:  shifting LEDs
	uint8_t LED_vals[7]={0,0,0,0,0,0,0};
	for (uint8_t i=0;i<7;i++) SETBIT(PORTD,LED_map[i],0);
	uint8_t LED_active=0;

	while(1) {
		for (uint8_t i=0;i<7;i++) {
			if (i==LED_active) {
				LED_vals[i]++;
				if (LED_vals[i]==255) LED_active=LED_active==6?0:LED_active+1;
			} else
			if (LED_vals[i]>0) LED_vals[i]--;
		}

		for (uint8_t i=0;i<7;i++) {
			OCR0A = LED_vals[i];
			for (uint8_t j=0;j<7;j++) if (i==j) SETBIT(PORTD,LED_map[j],0) else SETBIT(PORTD,LED_map[j],1)
			_delay_us(100);
		}
	}

}

