#pragma once

#include "i2c.h"

unsigned char TAOS_address=0x29<<1;

///Command byte
#define TAOS_CMD 0b10000000
#define TAOS_AUTOINCR 0b00100000
#define TAOS_SPECIAL 0b01100000

///Registers
#define TAOS_ENABLE 0x00
#define TAOS_ATIME 0x01
#define TAOS_WTIME 0x03
#define TAOS_AILTL 0x04
#define TAOS_AIHTL 0x05
#define TAOS_AILTH 0x06
#define TAOS_AIHTH 0x07
#define TAOS_PERS 0x0C
#define TAOS_CONFIG 0x0D
#define TAOS_CONTROL 0x0F
#define TAOS_ID 0x12
#define TAOS_STATUS 0x13
#define TAOS_CDATAL 0x14
#define TAOS_CDATAH 0x15
#define TAOS_RDATAL 0x16
#define TAOS_RDATAH 0x17
#define TAOS_GDATAL 0x18
#define TAOS_GDATAH 0x19
#define TAOS_BDATAL 0x1A
#define TAOS_BDATAH 0x1B

///Enable register options
#define TAOS_ENABLE_AIEN 0b00010000
#define TAOS_ENABLE_WEN  0b00001000
#define TAOS_ENABLE_AEN  0b00000010
#define TAOS_ENABLE_PON  0b00000001

#define TAOS_GAIN_1X 0
#define TAOS_GAIN_4X 1
#define TAOS_GAIN_16X 2
#define TAOS_GAIN_40X 3

unsigned char taos_read_register(unsigned char reg) {
	return i2c_read_register(TAOS_address,reg+TAOS_CMD);
}

bool taos_write_register(unsigned char reg, unsigned char data) {
	return i2c_write_register(TAOS_address,reg+TAOS_CMD,data);
}

///Checks if sensor is accessible
///@returns 1 on success
bool taos_detect() {
	if(i2c_write_register(TAOS_address,0,0)) //no ack
		return 0;
	
	if(taos_read_register(TAOS_ID)!=0x44) //ID doesn't match
		return 0;
	
	return 1;
}

void taos_enable(bool on) {
	unsigned char enable=taos_read_register(TAOS_ENABLE);
	change_bit(enable,TAOS_ENABLE_PON,TAOS_ENABLE_PON*on);
	taos_write_register(TAOS_ENABLE,enable);
	__delay_cycles(1000);
	change_bit(enable,TAOS_ENABLE_AEN,TAOS_ENABLE_AEN*on);
	taos_write_register(TAOS_ENABLE,enable);
}

#define TAOS_CLEAR TAOS_CDATAH
#define TAOS_RED TAOS_RDATAH
#define TAOS_GREEN TAOS_GDATAH
#define TAOS_BLUE TAOS_BDATAH
unsigned int taos_read(unsigned char channel) {
	unsigned int data;
	data=taos_read_register(channel)<<8;
	data+=taos_read_register(channel-1);
	return data;
}