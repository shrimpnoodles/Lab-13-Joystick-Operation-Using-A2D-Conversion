/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/file/d/1mdz4U-bm3RJGTe72yOyShQztUu5gsfNT/view?usp=sharing
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

void ADC_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//ADEN: SETTING THIS BIT ENABLES ANALOG TO DIGITAL CONVERSION
	//ADSC: SETTING THIS BIT STARTS THE FIRST CONVERSION
	//ADATE: SETTING THIS BIT ENABLES AUTO TRIGGERING
	//	SINCE WE ARE IN FREE RUNNING MODE A NEW CONVERSION
	//	WILL TRIGGER WHENEVER THE PREVIOUS CONVERSION COMPLETES
}

//unsigned short x;
unsigned char pattern;

enum SM1_States {start1,init, nomove, right, left} state;

void SM1_Tick(){
	switch(state){
		case start1:
			state = init;
			break;
		case init:
			state = nomove;
			break;
		case nomove:
			if(ADC  > 512+100){
				state = right;
			}
			else if(ADC  < 512-100){
				state = left;
			}
			break;
		case right:
			 if(ADC <512-100){
				state = left;
			}
			// else if(ADC >512+100){
			//	 state = right;
			// }
			 else{
				 state = nomove;
			 }
			break;
		case left:
			 if(ADC  >512+100){
				state = right;
			}
			// else if(ADC < 512 -100){
			//	 state = left;
			// }
			 else {
				 state = nomove;
			 }
			break;
		default:
			state= start1;
			break;
	}
	switch(state){
		case start1:
			break;
		case init:
			PORTD = 0x7f;
			pattern = 0x01;
			PORTC = pattern;
		case nomove:
			if(pattern == 0x80 && ADC < 512-100){
				pattern = 0x01;
			}
			PORTC = pattern;		
			break;
		case right:
			if(pattern ==0x01 ){
				pattern =0x80;
			}
			else{
				pattern = pattern >> 1;
			}
			PORTC = pattern;
			break;
		case left:
			if(pattern ==0x80){
				pattern ==0x01;
			}
			else{
				pattern = pattern << 1;
			}
			PORTC = pattern;
			break;
		default:
			break;
	}
}
/*
enum output_States{output} state_2;

void output_Tick(){
	switch(state_2){
		case output:
			break;
		default:
			state = output;
			break;
		}
	switch(state_2){
		case output:
			PORTD = 0x7f;
		//	PORTD =0XFE;
	//		PORTD = 0x00;	
			PORTC = pattern;
			break;
		default:
			break;
	}
}*/

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA =0X00; PORTA = 0XFF;
	DDRC = 0XFF; PORTC = 0X00;
	DDRD = 0XFF; PORTD = 0X00;
	
//	pattern = 0x01;

	void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
	ADC_init();


//	unsigned short
//	 x = ADC;
//	unsigned char my_char;
    /* Insert your solution below */
	state = start1;
//	state_2 =output;
	
	TimerSet(10);
	TimerOn();


    while (1) {
//	    x = ADC;
	SM1_Tick();
//	output_Tick();
	
	while(!TimerFlag){}
	TimerFlag = 0;

//	    x = ADC;
//	    my_char = (char)(x>>8)
//	    PORTD = (char)(x>>8);
	  //  my_char = (char)x;
//	    PORTB= (char)x;
    }
    return 1;
}
