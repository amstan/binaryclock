#include <msp430.h>
#include "bitop.h"
//#include "USBSerial.h"

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
	core_frequency_set(16000000);
	//Change pin modes for 32k xtal
	P5SEL=0;
	set_bit(P5SEL,4);
	set_bit(P5SEL,5);
	
	#define BOARD_LED 3 //PJ.3
	PJDIR=1<<BOARD_LED;
	
	#define LED_PWM 3 //P1.3
	P1DIR|=(1<<LED_PWM);
	
	#define PWR_EN 1 //P5.1
	P5DIR|=(1<<PWR_EN);
	
	#define LED_DATA 1 //P4.1
	P4DIR|=(1<<LED_DATA);
	clear_bit(P4OUT,LED_DATA);
	
	#define SCL
	#define SDA
}

void calendar_init(void) {
	RTCCTL01=RTCMODE+RTCSSEL_0;
}

extern "C" {
	void write_ws2811_hs(uint8_t *data, uint16_t length, uint8_t pinmask);
}

int main(void) {
	chip_init();
// 	USBSerial_open();
	calendar_init();
	
	for(unsigned int i=0;i<100;i++) __delay_cycles(60000); //wait for led controllers to start up
	
	set_bit(PJOUT,BOARD_LED);
	set_bit(P5OUT,PWR_EN);
	set_bit(P1OUT,LED_PWM);
	
	#define G 0
	#define R 1
	#define B 2
	const uint16_t led_count=18;
	uint8_t data[led_count*3]={
		0x00, 0xff, 0x00,
		0xff, 0xff, 0x00,
		0xff, 0x00, 0x00,
		0xff, 0x00, 0xff,
		0x00, 0x00, 0xff,
		0x00, 0xff, 0xff,
		
		0x00, 0x40, 0x00,
		0x40, 0x40, 0x00,
		0x40, 0x00, 0x00,
		0x40, 0x00, 0x40,
		0x00, 0x00, 0x40,
		0x00, 0x40, 0x40,
		
		0x00, 0x08, 0x00,
		0x08, 0x08, 0x00,
		0x08, 0x00, 0x00,
		0x08, 0x00, 0x08,
		0x00, 0x00, 0x08,
		0x00, 0x08, 0x08,
	};
	
	#define HOURS 0
	#define MINUTES 1
	#define SECONDS 2
	
	RTCHOUR=22;
	RTCMIN=23;
	RTCSEC=15;
	
	write_ws2811_hs(data,led_count*3,1<<LED_DATA);
	while(1) {
		for (unsigned char row=0;row<3;row++) {
			unsigned char row_R,row_G,row_B,row_value;
			switch(row) {
				case HOURS:
					row_R=0x01;
					row_G=0x00;
					row_B=0x00;
					row_value=RTCHOUR;
					break;
				case MINUTES:
					row_R=0x00;
					row_G=0x01;
					row_B=0x00;
					row_value=RTCMIN;
					break;
				case SECONDS:
					row_R=0x00;
					row_G=0x00;
					row_B=0x01;
					row_value=RTCSEC;
					break;
			}
			for (unsigned char inv_bit=0;inv_bit<6;inv_bit++) {
				unsigned char led=row*6+inv_bit;
				unsigned char bit=5-inv_bit;
				data[led*3+R]=row_R*(test_bit(row_value,bit)*10+1);
				data[led*3+G]=row_G*(test_bit(row_value,bit)*10+1);
				data[led*3+B]=row_B*(test_bit(row_value,bit)*10+1);
			}
		}
		write_ws2811_hs(data,led_count*3,1<<LED_DATA);
		for(unsigned int i=0;i<1;i++)
			__delay_cycles(6000);
// 		printf("%02u:%02u:%02u\n",RTCHOUR,RTCMIN,RTCSEC);
	}
}