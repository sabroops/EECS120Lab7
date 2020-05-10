/*	Author: me
 *  Partner(s) Name: game with display
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: hit middle light and reach nine
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



enum States{Start, S0, S1, S2, S3, Bad_Press, Good_Press, Wait_Press, Wait_Release, WINNER} state;
unsigned char  tmpA = 0;
unsigned char tmpB = 0x00;
unsigned char score = 0x00;
unsigned char cnt = 0;
unsigned char disrespect = 0;

void Tick(){
	switch(state){
		case Start:
			state=S0;
			break;
		case S0:
			if (tmpA){
				state = Bad_Press;
			}
			else if (cnt %4 > 0) {state = S1;}
			else {state = S0;}
			break;
		case S1:
			if (tmpA){
				state = Good_Press;
			}
			else if (cnt %4 > 1 ) {state = S2;}
			else {state = S1;}
                        break;
		case S2:
			if (tmpA){
				state = Bad_Press;
			}
			else if (cnt % 4 > 2) {state = S3;}
                        else {state = S2;}
                        break;
			break;
		case S3:
			if (tmpA){
				state = Good_Press;
			}
			else if (cnt % 4 == 0) {state = S0;}
                        else {state = S3;}
			break;
		case Bad_Press:
			if (tmpA) {state = Wait_Press;}
			else if (!tmpA){state = Wait_Release;}
			break;
		case Good_Press:
			if (tmpA) {state = Wait_Press;}
			else if (!tmpA){state = Wait_Release;}
			break;
		case Wait_Press: 
			if (!tmpA){state = Wait_Release;}
			break;
		case Wait_Release:
			if (score == 9) {state = WINNER;}
			if (tmpA) {state = S0; disrespect = 1;} 
			break;
		case WINNER:
			state = WINNER;
			break;
		default:
			state = Start;
			break;

	}

	switch(state){ //ACTIONS
		case Start:
			tmpB = 0x00;
			break;
		case S0:
			tmpB = 0x01;
			cnt++;
			break;
		case S1: 
			tmpB = 0x02;
			cnt++;
			break;
                case S2:
			tmpB = 0x04;
			cnt++;
			break;
                case S3:
			tmpB = 0x02;
			cnt++;
			break;
		case Bad_Press:
			if(score > 0) {score--;}
			break;
		case Good_Press:
			if (score < 9){
				score++;
			}
			break;
		case Wait_Press:
			break;
		case Wait_Release:
			cnt = 0;
			break;
		case WINNER:
			score = 100;
			break;
		default:
			break;

	}

}

int main(void) {
    /* Insert DDR and PORT initializations */
	PORTA = 0xFF; DDRA = 0x00;
        PORTB = 0x00; DDRB = 0xFF;
	PORTC = 0x00; DDRC = 0xFF;
	PORTD = 0x00; DDRD = 0xFF;
    /* Insert your solution below */
//	state = Start;
	unsigned char* ok = 0;
        //unsigned char tmpC = 0x00;
	TimerSet(300);
	TimerOn();

    LCD_init();

    while (1) {
	tmpA = ~PINA;
	if (tmpA == 0){disrespect = 0;}
	else if (disrespect == 1 && tmpA == 1){tmpA =0;}
	Tick();
	PORTB = tmpB;
	*ok = '0' +score;
	if(score == 100){
		LCD_DisplayString(1, "WINNER!!!");
	}
	else{
		LCD_DisplayString(1,ok);
	}	
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
