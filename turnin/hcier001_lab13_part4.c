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
#include "timer.h"
#include "scheduler.h"
#endif

void ADC_init(){
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//ADEN: SETTING THIS BIT ENABLES ANALOG TO DIGITAL CONVERSION
	//ADSC: SETTING THIS BIT STARTS THE FIRST CONVERSION
	//ADATE: SETTING THIS BIT ENABLES AUTO TRIGGERING
	//	SINCE WE ARE IN FREE RUNNING MODE A NEW CONVERSION
	//	WILL TRIGGER WHENEVER THE PREVIOUS CONVERSION COMPLETES
}

// Pins on PORTA are used as input for A2D conversion
	//    The default channel is 0 (PA0)
	// The value of pinNum determines the pin on PORTA
	//    used for A2D conversion
	// Valid values range between 0 and 7, where the value
	//    represents the desired pin for A2D conversion
	void Set_A2D_Pin(unsigned char pinNum) {
		ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
		// Allow channel to stabilize
		static unsigned char i = 0;
		for ( i=0; i<15; i++ ) { asm("nop"); } 
}

unsigned char pattern;
unsigned char row;

enum leftright_States{start1, init, nomove, left, right};

int leftright_Tick(int state){
	switch(state){
		case start1:
			state = init;
			break;
		case init:
			state = nomove;
			break;
		case nomove:
			if(ADC > 512 +100){
				state = right;
			}
			else if(ADC < 512-100){
				state = left;
			}
			break;
		case right:
			if(ADC < 512-100){
				state = left;
			}
			else{
				state = nomove;
			}
			break;
		case left:
			if(ADC > 512+100){
				state = right;
			}
			else{
				state = nomove;
			}
			break;
		default:
			state = start1;
			break;
		}
	switch(state){
		case start1:
		//	Set_A2D_Pin(0x00);
			break;
		case init:
			pattern = 0x01;
			PORTC = pattern;
			break;
		case nomove:
			PORTC = pattern;
			break;
		case right:
			if(pattern ==0x01){
				pattern = 0x01;
			}
			else{
				pattern = pattern >>1;
			}
			PORTC = pattern;
			break;
		case left:
			if(pattern == 0x80){
				pattern = 0x80;
			}
			else{
				pattern = pattern <<1;
			}
			PORTC = pattern;
			break;
		default:
			break;
	}
	return state;
}

enum updown_States{start2, init2, nomove2, up, down};

int updown_Tick(int state){
	switch(state){
		case start2:
			state = init2;
			break;
		case init2:
			state = nomove2;
			break;
		case nomove2:
			if(ADC > 512+100){
				state = up;
			}
			else if(ADC < 512-100){
				state = down;
			}
			break;
		case up:
			if(ADC < 512-100){
				state = down;
			}
			else {
				state = nomove2;
			}
			break;
		case down:
			if(ADC > 512+100){
				state = up;
			}
			else{
				state = nomove2;
			}
			break;
		default:
			state = start2;
			break;
		}
	switch(state){
		case start2:
		//	Set_A2D_Pin(0x01);
			break;
		case init2:
//			PORTC = 0x01;
			row = 0x7f;
			PORTD = row;
			break;
		case nomove2:
			PORTD = row;
			break;
		case up:
		//	if(row == 0x7f){
		//		row = 0x7f;
		//	}
			 if(row == 0xfe){
				row = 0xfd;
			}
			else if(row == 0xfd){
				row = 0xfb;
			}
			else if(row == 0xfb){
				row = 0xf7;
			}
			else if(row == 0xf7){
				row = 0xef;
			}
			else if(row == 0xef){
				row = 0xdf;
			}
			else if(row == 0xdf){
				row = 0xbf;
			}
			else if(row == 0xbf){
				row = 0x7f;
			}
		//	else{
		//		row = (row >>1)|0x80;
		//	}
			PORTD = row;
			break;
		case down:
		//	if(row == 0xfe){
		//		row = 0xfe;
		//	}
			 if(row == 0x7f){
				row = 0xbf;
			}
			else if(row == 0xbf){
				row = 0xdf;
			}
			else if(row == 0xdf){
				row = 0xef;
			}
			else if(row == 0xef){
				row = 0xf7;
			}
			else if(row == 0xf7){
				row = 0xfb;
			}
			else if(row == 0xfb){
				row = 0xfd;
			}
			else if(row == 0xfd){
				row = 0xfe;
			}
		//	else{
		//		row = (row <<1)|0x01;
		//	}
			PORTD = row;
			break;
		default:
			break;
	}
	return state;
}

enum seta2d_States{LR, UD};

int seta2d_Tick(int state){
	switch(state){
		case LR:
			state = waitlr;
			break;
		case UD:
			state = waitud;
			break;
		default:
			state = LR;
			break;
		}
	switch(state){
		case LR:
			Set_A2D_Pin(0x00);
			break;
		case UD:
			Set_A2D_Pin(0x01);
			break;;
		default:
			break;
		}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA =0X00; PORTA = 0XFF;
	DDRC = 0XFF; PORTC = 0X00;
	DDRD = 0XFF; PORTD = 0X00;
	
		

	void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
	ADC_init();

	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &leftright_Tick;

	
	task2.state = start;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &updown_Tick;

	task3.state = start;
	task3.period = 50;
	task3.elapsedTime = task2.period;
	task3.TickFct = &seta2d_Tick;

	unsigned long int findGCD(unsigned long int a, unsigned long int b){
		unsigned long int c;
		while(1){
			c = a%b;
			if (c==0){
				return b;
			}
			a = b;
			b = c;
		}
		return 0;
	}
	unsigned short i;
	unsigned long GCD=tasks[0]->period;
	for(i=1; i< numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}
//	GCD = 1;	
	TimerSet(GCD);
	TimerOn();

    	while (1) {
		for(i=0; i<numTasks; i++){
		if(tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}
		TimerSet(1);
	       	while(!TimerFlag);
	       	TimerFlag = 0;
	       	TimerSet(GCD);
	    }
	    return 1;
    }

