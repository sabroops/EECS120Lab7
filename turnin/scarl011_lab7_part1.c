/*	Author: me
 *  Partner(s) Name: 123123123123123
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *	MAKE BOARD GO 123123123123 switch every sec!!
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include<avr/interrupt.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR (TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr ==0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


enum States{Start, S0, S1, S1_2, S2, S2_2, S3} state;
unsigned char A = 0x00;
unsigned char tmpC = 0x00;
unsigned char count = 0x00;

void Tick(){
	switch(state){
		case Start:
			state=S0;
			break;
		case S0:
			count = 0;
			if(A == 0x01){ state = S1;} //INCREMENT
			else if (A == 0x02) { state = S2;} //DECREMENT
			else if (A == 0x03) { state = S3;} //RESET!
			else if (A == 0x00) { state = S0;} //STAY THE SAME
			break;
		case S1:
                        if(A == 0x01){state = S1_2; count++;} //INCREMENT
                        else if (A == 0x02) {} //DECREMENT
                        else if (A == 0x03) { state = S3;} //RESET!
                        else if (A == 0x00) { state = S0;} //STAY THE SAME
                        break;
		case S1_2:
                        if(A == 0x01){state = S1_2; count++;} //INCREMENT
                        else if (A == 0x02) {} //DECREMENT
                        else if (A == 0x03) { state = S3;} //RESET!
                        else if (A == 0x00) { state = S0;} //STAY THE SAME
                        break;
		case S2:
                        if(A == 0x01){} //INCREMENT
                        else if (A == 0x02) {state = S2_2; count++;} //DECREMENT
                        else if (A == 0x03) { state = S3;} //RESET!
                        else if (A == 0x00) { state = S0;} //STAY THE SAME
                        break;
		case S2_2:
                        if(A == 0x01){} //INCREMENT
                        else if (A == 0x02) {state = S2_2; count++;} //DECREMENT
                        else if (A == 0x03) { state = S3;} //RESET!
                        else if (A == 0x00) { state = S0;} //STAY THE SAME
                        break;
		case S3:
			count = 0;
                        if(A == 0x01){} //INCREMENT
                        else if (A == 0x02) {} //DECREMENT
                        else if (A == 0x03) { state = S3;} //RESET!
                        else if (A == 0x00) { state = S0;} //STAY THE SAME
                        break; 

		default:
			state = Start;
			break;
	}
	switch(state){ //ACTIONS
		case Start:
			tmpC = 0x07;
			break;
		case S0:
			tmpC = tmpC;
			break;
		case S1: 
			if (tmpC == 0x09){
				tmpC = 0x09;
			}
			else{
				tmpC = tmpC + 1;
			}
			break;
		case S1_2:
			if ((tmpC < 0x09) && (count % 11 == 0)){
				tmpC++;
			}
			tmpC = tmpC;
			break;
                case S2:
			if (tmpC == 0x00){
				tmpC = 0x00;
			}
			else{
				//tmpC--;
			}
		case S2_2:
			if ((tmpC > 0x00) && (count%11 == 0)){
				tmpC--;
			} //ugh!
			tmpC = tmpC;
			break;
                case S3:
                        tmpC = 0x00;
                        break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	PORTA = 0xFF; DDRA = 0x00;
	PORTC = 0x00; DDRC = 0xFF;
	PORTD = 0x00; DDRD = 0xFF;
    /* Insert your solution below */
//	state = Start;
	unsigned char* ok = 0;
        //unsigned char tmpC = 0x00;
	TimerSet(100);
	TimerOn();

    LCD_init();

    while (1) {
	A = ~PINA;
	Tick();
	*ok = '0' +tmpC;
	LCD_DisplayString(1,ok);
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
