/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/file/d/1nrM6wg1RDAGFbfJhe6qH7H9u9f3F9oY1/view?usp=sharing
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

//unsigned short x;
//
unsigned char  tempspeed;
unsigned char temporary;
enum speed_States{startS, increase, decrease};

int speed_Tick(int state){
	switch(state){
		case startS:
			if(ADC > 512+100){
				state = increase;
			}
			else if(ADC < 512 - 100){
				state = decrease;
			}
			break;
		case increase:
			if(ADC < 512-100){
				state = decrease;
			}

			break;
		case decrease:
			if(ADC > 512+100){
				state = increase;
			}
		default:
			state = startS;
			break;
		}
	switch(state){
		case startS:
			break;
		case increase:
			 if(ADC >= 900){
				tempspeed = 10;
			}
			else if(ADC >= 800 && ADC < 900){
				tempspeed = 25;
			}
			else if(ADC >= 700 && ADC < 800){
				tempspeed = 50;
			}
			else if(ADC >= 600 && ADC < 700){
				tempspeed = 100;
				}
			break;
		case decrease:
			if(ADC <= 100){
				tempspeed = 10;
			}
			else if(ADC <= 200 && ADC > 100){
			       tempspeed = 25;
			}
	 		else if(ADC <= 300 && ADC > 200){
				tempspeed=50;
			}
			else if(ADC <= 400 && ADC > 300){
				tempspeed =100;
			}
			
			break;	
		default:
			break;
		}
	return state;
}

unsigned char pattern;

enum SM1_States {start1,init, nomove, right, waitR, left, waitL};

int SM1_Tick(int state){
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
			/* if(ADC <512-100){
				state = left;
			}
			 else{
				 state = nomove;
			 }*/
			if(ADC > 512+100){
			state = waitR;
			}
			else {
				state = nomove;
			}
			break;
		case waitR:
			if(ADC < 512 -100){
				state = left;
			}
			else if(ADC > 512 +100 && temporary ==0){
				state = right;
			}
			break;
		case left:
			/* if(ADC  >512+100){
				state = right;
			}
			 else {
				 state = nomove;
			 }*/
			if(ADC < 512-100){
			state = waitL;
			}
			else {
				state = nomove;
			}
			break;
		case waitL:
			if(ADC > 512+100){
				state = right;
			}
			else if(ADC < 512-100 && temporary ==0){
				state = left;
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
			temporary = tempspeed;
			if(pattern == 0x80 && ADC < 512-100){
				pattern = 0x01;
			}
			PORTC = pattern;		
			break;
		case right:
			temporary = tempspeed;
			if(pattern ==0x01 ){
				pattern =0x80;
			}
			else{
				pattern = pattern >> 1;
			}
			PORTC = pattern;
			break;
		case waitR:
			temporary--;
			break;
		case left:
			temporary = tempspeed;
			if(pattern ==0x80){
				pattern =0x01;
			}
			else{
				pattern = pattern << 1;
			}
			PORTC = pattern;
			break;
		case waitL:
			temporary--;
			break;
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
	
//	tempspeed = 10;

	void A2D_init() {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
	ADC_init();

	static task task1, task2;
	task *tasks[] = {&task1, &task2};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &speed_Tick;

	task2.state = start;
	task2.period = 1;
	task2.elapsedTime = task2.period;
	task2.TickFct = &SM1_Tick;

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
	unsigned long GCD = tasks[0]->period;
	for(i=1; i< numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
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

	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
    }

