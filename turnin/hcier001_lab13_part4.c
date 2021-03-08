/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/file/d/1UPl9Ps33NoAMsqUzbkmxrhoz8-N6F1lM/view?usp=sharing
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char pattern = 0x80;
unsigned char row = 0xFE;

void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	static unsigned char i = 0;
	for (i = 0; i < 15; i++) { asm("nop"); }
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//ADEN: setting this bit enables analog-to-digital conversion
	//ADSC: setting this bit starts the first conversion.
	//ADATE: setting this bit enables auto-triggering. Since we are in
			// Free Running Mode, a new conversion will trigger whenever the previous conversion completes
}
enum leftright_States{start1, init, nomove, left, right};

int leftright_Tick(int state){

	Set_A2D_Pin(0x00);
	switch(state) {
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
	//		Set_A2D_Pin(0x00);
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
	}
	PORTC = pattern;
	PORTD = row;
	return state;
}

enum updown_States{start2, init2, nomove2, up, down};

int updown_Tick(int state){
	Set_A2D_Pin(0x01);
	switch(state) {
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
	switch(state) {
		case start2:
	//		Set_A2D_Pin(0x02);
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
	PORTC = pattern;
	PORTD = row;
	return state;
}

enum seta2d_States{starta2d, LR, UD};

int seta2d_Tick(int state){
	switch(state){
		case starta2d:
			state = LR;
			break;
		case LR:
			state = UD;
			break;
		case UD:
			state = LR;
			break;
		default:
			state = starta2d;
			break;
		}
	switch(state){
		case starta2d:
			break;
		case LR:
			Set_A2D_Pin(0x00);
			break;
		case UD:
			Set_A2D_Pin(0x02);
			break;
		default:
			break;
		}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();

	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	
	task1.state = start;
	task1.period = 10;
	task1.elapsedTime = task1.period;
	task1.TickFct = &leftright_Tick;
	
	task2.state = start;
	task2.period = 10;
	task2.elapsedTime = task2.period;
	task2.TickFct = &updown_Tick;
	
	task3.state = start;
	task3.period = 1;
	task3.elapsedTime = task3.period;
	task3.TickFct = &seta2d_Tick;

	TimerSet(1);
	TimerOn();

	unsigned short i;

	while(1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i] -> elapsedTime == tasks[i] -> period) {
				tasks[i] -> state = tasks[i] -> TickFct(tasks[i] -> state);
				tasks[i] -> elapsedTime = 0;
			}
			tasks[i] -> elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;

}
