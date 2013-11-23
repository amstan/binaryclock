#include <msp430.h>
#include "bitop.h"

#include <stdlib.h>

#include "util.h"

void chip_init(void) {
	WDTCTL = WDTPW + WDTHOLD; //Stop WDT
	
	//Change pin modes for 32k xtal
	P5SEL=0;
	set_bit(P5SEL,4);
	set_bit(P5SEL,5);
	
	//change pins for hfxtal(4MHz), needed for usb
	set_bit(P5SEL,2);
	set_bit(P5SEL,3);
	
	//FCPU setting
	core_frequency_set(16000000); //uses 32KHz
	
	///The green led from the olimexino board
	#define BOARD_LED 3 //PJ.3
	PJDIR=1<<BOARD_LED;
	
	///SMPSU
	#define PWR_EN 1 //P5.1
	set_bit(P5DIR,PWR_EN);
	clear_bit(P5OUT,PWR_EN);
	
	///N-Channel PWMing mosfet for the 5V that powers the led diodes
	#define LED_PWM 3 //P1.3
	set_bit(P1DIR,LED_PWM);
	clear_bit(P1OUT,LED_PWM);
	
	///Data pin for the WS2812s
	#define LED_DATA 1 //P4.1
	P4DIR=(1<<LED_DATA);
	clear_bit(P4OUT,LED_DATA);
	
	///I2C for the TAOS light sensor
	#define SCL 0 //P1.0
	#define SDA 1 //P1.1
	#define I2COUT P1OUT
	#define I2CIN P1IN
	#define I2CDIR P1DIR
	
	#define CBOUT 6 //P1.6
	set_bit(P1SEL,CBOUT);
	set_bit(P1DIR,CBOUT);
	
	///Capacitive Touch - Port 6
	#define RIGHT 0
	#define UP 1
	#define DOWN 2
	#define LEFT 3
	
	_BIS_SR(GIE);
}

#include "ws2811.h"

uint8_t white[18*3];

#include "calendar.h"
#include "usb-cdc.h"
#include "capacitive_touch.h"
#include "taos.h"

int main(void) {
	chip_init();
	USBSerial_open();
	calendar_init();
	
	for(unsigned int i=0;i<100;i++) __delay_cycles(60000); //wait for led controllers to start up
	
	clear_bit(PJOUT,BOARD_LED);
	set_bit(P5OUT,PWR_EN);
	set_bit(P1OUT,LED_PWM);
	
	const uint16_t led_count=18;
	#define n (led_count*3)
	uint8_t dest[n];
	uint8_t current[n];
	memset(white,0xff,18*3);
	
	unsigned char calib_cap[4];
	for(unsigned char j=0;j<10;j++) {
		for(unsigned char i=0;i<4;i++) {
			calib_cap[i]=capacitance_read(i)+20;
		}
	}
	
	while(1) {
		while(USBSerial_read()!='\n');
		printf("\nTesting i2c:\n");
		printf("Writing test: ACK %d\n",i2c_write_register(TAOS_address, TAOS_AILTL,0xff));
// 		printf("ENABLE= %x\n",i2c_read_register(TAOS_address, TAOS_ENABLE));
// 		printf("WTIME= %x\n",i2c_read_register(TAOS_address, TAOS_WTIME));
// 		printf("ID= %x\n",i2c_read_register(TAOS_address, TAOS_ID));
	}
}