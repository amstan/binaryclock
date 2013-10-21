#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "bitop.h"

unsigned char button=0;
#define BUTTONPIN 0

#define REFRESHBIT 4

volatile unsigned char display[3]={0,0,0};
unsigned char tempDDRA, tempDDRB, tempPORTA, tempPORTB;
void refreshports() {
	//Make everything an input for safety
	DDRA=0;
	DDRB=0;
	
	//Enable ports first so shorts are avoided because of the pullup resistors
	PORTA=tempPORTA;
	PORTB=tempPORTB;
	
	//Enable real DDR
	DDRA=tempDDRA;
	DDRB=tempDDRB;
	
	if(!test_bit(DDRA,BUTTONPIN)) { //If BUTTONPIN is an input
		_delay_ms(1);
		button=(test_bit(PINA,BUTTONPIN)==0); //read button
	}
}

unsigned char bitswap(unsigned char s) {
	unsigned char i,d=0;
	
	#define BITS 6
	for(i=0;i<BITS;i++)
		change_bit(d,i,test_bit(s,BITS-1-i));
	
	return d;
}

const unsigned char COLUMNMASK=0b00111111;
void setcolumn(unsigned char column) {
	column=bitswap(column);
	column&=COLUMNMASK;
	
	//clear column
	tempPORTA&=~COLUMNMASK;
	
	//set only the bits needed
	tempPORTA|=column;
}

void setrow(char row) {
	//change to highZ
	tempDDRB  =0;
	tempPORTB =0;
	tempDDRA  =COLUMNMASK;
	tempPORTA =0;
	
	switch(row) {
		case 0: //button stuff row
			clear_bit(tempDDRA,BUTTONPIN); //input
			set_bit(tempPORTA,BUTTONPIN);  //pullup
			set_bit(tempDDRA,6);           //change row
			break;
		case 1:
			set_bit(tempDDRA,7);
			break;
		case 2:
			set_bit(tempDDRB,2);
			break;
		default:
			break;
	}
}

void initio(void) {
	setrow(-1);
	setcolumn(0);
	refreshports();
}

void inittimer(void) {
	// Start timer, 1x prescaler
	TCCR1B |= (1 << CS10);
	
	// Configure timer 1 for CTC mode
	TCCR1B |= (1 << WGM12);
	
	//Set compare value so it triggers at 1 second
	OCR1A = F_CPU;
	
	//enable interrupts
	TIMSK1 = (1 << OCIE1A); // Enable CTC interrupt
	sei(); //global
}

#ifdef HOUR
unsigned char sec=SEC;
unsigned char min=MIN;
unsigned char hour=HOUR;
#else
unsigned char sec=0;
unsigned char min=0;
unsigned char hour=0;
#endif
//One second interrupt
ISR(TIM1_COMPA_vect) {
	sec++;
	sec%=60;
	if(sec==0) {
		min++;
		min%=60;
		if(min==0) {
			hour++;
			hour%=24;
		}
	}
	
	display[0]=hour;
	display[1]=min;
	display[2]=sec;
}

void main(void) {
	initio();
	inittimer();
	
	unsigned char r;
	while(1) {
		for(r=0;r<3;r++) {
			setrow(r);
			setcolumn(display[r]);
			
			//Wait for raising edge of REFRESHBIT
			//while(!test_bit(TCNT1,REFRESHBIT));
			//while(test_bit(TCNT1,REFRESHBIT));
			
			refreshports();
		}
	}
	
	while(1);
}
