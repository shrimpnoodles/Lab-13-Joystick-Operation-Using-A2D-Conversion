/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//ADEN: SETTING THIS BIT ENABLES ANALOG TO DIGITAL CONVERSION
	//ADSC: SETTING THIS BIT STARTS THE FIRST CONVERSION
	//ADATE: SETTING THIS BIT ENABLES AUTO TRIGGERING
	//	SINCE WE ARE IN FREE RUNNING MODE A NEW CONVERSION
	//	WILL TRIGGER WHENEVER THE PREVIOUS CONVERSION COMPLETES
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA =0X00; PORTA = 0XFF;
	DDRB = 0XFF; PORTB = 0X00;
	DDRD = 0XFF; PORTD = 0X00;

	void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
	ADC_init();
	unsigned short x = ADC;
//	unsigned char my_char;
    /* Insert your solution below */


    while (1) {
	    x = ADC;
//	    my_char = (char)(x>>8)
	    PORTD = (char)(x>>8);
	  //  my_char = (char)x;
	    PORTB= (char)x;
    }
    return 1;
}
