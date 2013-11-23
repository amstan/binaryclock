#pragma once

void calendar_init(void) {
	RTCCTL01=RTCMODE+RTCSSEL_0;
}

#define HOURS 0
#define MINUTES 1
#define SECONDS 2

void gettime(unsigned char *data) {
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
			data[led*3+R]=row_R*(test_bit(row_value,bit)*0x10+1);
			data[led*3+G]=row_G*(test_bit(row_value,bit)*0x10+1);
			data[led*3+B]=row_B*(test_bit(row_value,bit)*0x10+1);
		}
	}
}