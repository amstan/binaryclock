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
	//Enable weak pullup in case board is not connected
	set_bit(P1REN,SCL); set_bit(P1REN,SDA);
	clear_bit(I2CDIR,SCL); clear_bit(I2CDIR,SDA);
	set_bit(I2COUT,SCL); set_bit(I2COUT,SDA);
	
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

unsigned char glow=0;
bool glow_rising=1;

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
	const uint8_t rainbow[n] = {
		0x0c, 0x28, 0x0c,
		0x28, 0x28, 0x0c,
		0x28, 0x0c, 0x0c,
		0x28, 0x0c, 0x28,
		0x0c, 0x0c, 0x28,
		0x0c, 0x28, 0x28,
		
		0x00, 0x20, 0x00,
		0x20, 0x20, 0x00,
		0x20, 0x00, 0x00,
		0x20, 0x00, 0x20,
		0x00, 0x00, 0x20,
		0x00, 0x20, 0x20,
		
		0x00, 0x04, 0x00,
		0x04, 0x04, 0x00,
		0x04, 0x00, 0x00,
		0x04, 0x00, 0x04,
		0x00, 0x00, 0x04,
		0x00, 0x04, 0x04,
	};
	
	unsigned char calib_cap[4];
	for(unsigned char j=0;j<10;j++) {
		for(unsigned char i=0;i<4;i++) {
			calib_cap[i]=capacitance_read(i)+20;
		}
	}
	
	int r, g, b;
	int light_level;
	
	char mode=3;
	
	bool pl[]={0,0,0,0}; //last button states
	bool pc[]={0,0,0,0}; //current button states
	
	#define CLAMP(val,min,max) {if(val<min){val=min;};if(val>max){val=max;}}
	
	#ifdef COMPILE_HOUR
	RTCHOUR=COMPILE_HOUR;
	RTCMIN=COMPILE_MIN;
	RTCSEC=COMPILE_SEC;
	#endif
	
	unsigned char adjrow=1;
	
	while(1) {
		for(unsigned char i=0;i<4;i++) {
			pc[i]=capacitance_read(i)>(calib_cap[i]+80);
			#define PRESSED(button) ((pl[button]==0)&&(pc[button]==1))
			#define RELEASED(button) ((pl[button]==0)&&(pc[button]==1))
			if(PRESSED(i)) {
				//printf("%d pressed.\n",i);
			}
			if(RELEASED(i)) {
				//printf("%d released.\n",i);
				if(i==LEFT) {
					if(mode!=0) {
						mode--;
						if(mode==-1) mode=4;
						//printf("Mode %d",mode);
					}
				}
				if(i==RIGHT) {
					if(mode!=4) {
						mode++;
						if(mode==5) mode=0;
						//printf("Mode %d",mode);
					}
				}
			}
		}
		switch(mode) {
			case 0: //Capacitive touch demo
				set_bit(P5OUT,PWR_EN);
				for(unsigned int i=0;i<18;i++) {
					r=capacitance_read(3)-calib_cap[2];
					CLAMP(r,0,255);
					g=capacitance_read(2)-calib_cap[1];
					CLAMP(g,0,255);
					b=capacitance_read(1)-calib_cap[0];
					CLAMP(b,0,255);
					current[i*3+R]=r;
					current[i*3+G]=g;
					current[i*3+B]=b;
				}
				write_ws2811_hs(current,n,1<<LED_DATA);
				//printf("%d\t%d\t%d\t%d\n",capacitance_read(3),capacitance_read(2),capacitance_read(1),capacitance_read(0));
				break;
			
			case 1: //RGB leds rainbow
				set_bit(P5OUT,PWR_EN);
				for(unsigned int i=0;i<n;i++) {
					if(current[i]>rainbow[i]) current[i]--;
					if(current[i]<rainbow[i]) current[i]++;
				}
				write_ws2811_hs(current,n,1<<LED_DATA);
				break;
			
			case 2: //light sensor demo
				clear_bit(P5OUT,PWR_EN);
				taos_write_register(TAOS_ENABLE,TAOS_ENABLE_PON+TAOS_ENABLE_AEN);
				taos_write_register(TAOS_ATIME,0xf8); //integration time
				taos_write_register(TAOS_CONTROL,TAOS_GAIN_4X);
				taos_enable(1);
				
				memset(current,0,18*3);
				light_level=taos_read(TAOS_CLEAR);
				CLAMP(light_level,0,255);
				current[n-3]=(unsigned char)light_level;
				current[n-2]=(unsigned char)light_level;
				current[n-1]=(unsigned char)light_level;
				
				
				r=taos_read(TAOS_RED);
				g=taos_read(TAOS_GREEN);
				b=taos_read(TAOS_BLUE);
				CLAMP(r,0,255);
				CLAMP(g,0,255);
				CLAMP(b,0,255);
				current[n-6+R]=(unsigned char)r;
				current[n-6+G]=(unsigned char)g;
				current[n-6+B]=(unsigned char)b;
				
				current[n-21+R]=10;
				current[n-21+G]=10;
				current[n-21+B]=10;
				
				write_ws2811_hs(current,n,1<<LED_DATA);
// 				if(!taos_detect()) {
// 					printf("Could not detect taos sensor!\n");
// 				} else {
// 						printf("Found taos sensor!\n");
// 				}
// 				printf("r%u\tc%u\tr%u\tg%u\tb%u\n",taos_read(TAOS_CLEAR),light_level,r,g,b);
				break;
			
			case 3: //main clock
				gettime(dest,-1);
				for(unsigned int i=0;i<n;i++) {
					if(current[i]>dest[i]) current[i]--;
					if(current[i]<dest[i]) current[i]++;
				}
				write_ws2811_hs(current,n,1<<LED_DATA);
				break;
			
			case 4: //adj clock
				gettime(current,adjrow);
				
				volatile unsigned char *tochange;
				unsigned char maxvalue;
				switch(adjrow) {
					case 0: tochange=&RTCHOUR; maxvalue=24; break;
					case 1: tochange=&RTCMIN; maxvalue=60; break;
					case 2: tochange=&RTCSEC; maxvalue=60; break;
				}
				if(RELEASED(UP)) {
					(*tochange)++;
					if(*tochange>=maxvalue) {
						*tochange=0;
					}
				}
				if(RELEASED(DOWN)) {
					(*tochange)--;
					if(*tochange==0xFF) {
						*tochange=maxvalue-1;
					}
				}
				if(RELEASED(RIGHT)) {
					adjrow++;
					adjrow%=3;
				}
				
				write_ws2811_hs(current,n,1<<LED_DATA);
				break;
		}
		
		if(glow_rising) {
			glow++;
			if(glow==0x20) {
				glow_rising=0;
			}
		} else {
			glow--;
			if(glow==0x10) {
				glow_rising=1;
			}
		}
		
		for(unsigned char i=0;i<4;i++) {
			pl[i]=pc[i]; //save old touch values
		}
		
		for(unsigned int i=0;i<30;i++)
			__delay_cycles(6000);
	}
}
