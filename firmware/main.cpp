#include <msp430.h>
#include "bitop.h"
#include "USBSerial.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void core_frequency_set(unsigned long int frequency) {
	///Set multiplier based on the slow xtal
	UCSCTL3 |= SELREF_2;
	UCSCTL4 |= SELA_2;
	UCSCTL0 = 0x0000;
	UCSCTL1 = DCORSEL_5;
	UCSCTL2 = FLLD_1 + (frequency/32768)-1;
}

void chip_init(void) {
	WDTCTL = WDTPW + WDTHOLD; //Stop WDT
	
	//FCPU setting
	P5SEL |= 0x0C; //Change pin modes to xtal
	core_frequency_set(16000000);
	
	#define LED1 0
	P1DIR=1<<LED1;
	
	#define LED2 7
	P4DIR=1<<LED2;
}

int getchar(void) {
	return 0;
}

int putchar(int c) {
	return USBSerial_write(c);
}

int main(void) {
	chip_init();
	USBSerial_open();
	
	set_bit(P1OUT,LED1);
	clear_bit(P1OUT,LED1);
	
	//char buffer[50]="";
	unsigned int i=0;
	
	while(1) {
		
		toggle_bit(P1OUT,LED1);
		toggle_bit(P4OUT,LED2);
		
		printf("%u\n",i++);
		
		//USBSerial_write(buffer,strlen(buffer));
		
		i++;
	}
}