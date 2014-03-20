#pragma once

void calendar_init(void) {
	RTCCTL01=RTCMODE+RTCSSEL_0;
}

#define HOURS 0
#define MINUTES 1
#define SECONDS 2

void gettime(unsigned char *data,unsigned char highlight) {
	for (unsigned char row=0;row<3;row++) {
		unsigned char row_R,row_G,row_B;
		unsigned char row_value;
		unsigned char highlight_row;
		
		switch(row) {
			case HOURS:
				row_R=0x30;
				row_G=0x00;
				row_B=0x00;
				row_value=RTCHOUR;
				break;
			case MINUTES:
				row_R=0x00;
				row_G=0x30;
				row_B=0x00;
				row_value=RTCMIN;
				break;
			case SECONDS:
				row_R=0x00;
				row_G=0x00;
				row_B=0x30;
				row_value=RTCSEC;
				break;
		}
		
		if(highlight==row) {
			row_R+=glow;
			row_G+=glow;
			row_B+=glow;
		}
		
		for (unsigned char inv_bit=0;inv_bit<6;inv_bit++) {
			unsigned char led=row*6+inv_bit;
			unsigned char bit=5-inv_bit;
			data[led*3+R]=test_bit(row_value,bit)*row_R+1*(row_R>0)+2*(highlight==row);
			data[led*3+G]=test_bit(row_value,bit)*row_G+1*(row_G>0)+2*(highlight==row);
			data[led*3+B]=test_bit(row_value,bit)*row_B+1*(row_B>0)+2*(highlight==row);
		}
	}
}