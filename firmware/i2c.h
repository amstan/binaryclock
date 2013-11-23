#pragma once

///@title I2C software library
///Assumes SDA, SCL, I2COUT, I2CIN, I2CDIR are defined
///Code modified from the wikipedia example because i was lazy.
///This version of the code does not support arbitration or clock stretching

// Hardware-specific support functions that MUST be customized:
#define I2C_SPEED 1000
#define I2C_delay() __delay_cycles(I2C_SPEED)

#define I2C_READ_HOLDOFF_DELAY 1000
#define I2C_READ_HOLDOFF() __delay_cycles(I2C_READ_HOLDOFF_DELAY)

#define I2C_SLAVE_WRITE 0
#define I2C_SLAVE_READ 1

bool read_SCL(void) {
	// Set SCL as input and return current level of line, 0 or 1
	clear_bit(I2CDIR,SCL);
	I2C_READ_HOLDOFF();
	return test_bit(I2CIN,SCL);
}

bool read_SDA(void) {
	// Set SDA as input and return current level of line, 0 or 1
	clear_bit(I2CDIR,SDA);
	I2C_READ_HOLDOFF();
	return test_bit(I2CIN,SDA);
}

void clear_SCL(void) { 
	// Actively drive SCL signal low
	clear_bit(I2COUT,SCL);
	set_bit(I2CDIR,SCL);
}

void clear_SDA(void) { 
	// Actively drive SDA signal low
	clear_bit(I2COUT,SDA);
	set_bit(I2CDIR,SDA);
}

void arbitration_lost() {
	printf("\nError: I2C - Arbitration Lost!\n");
}
 
bool started = false; // global data
void i2c_start_cond(void) {
	if (started) { // if started, do a restart cond
		// set SDA to 1
		read_SDA();
		I2C_delay();
		while (read_SCL() == 0) {  // Clock stretching
		// You should add timeout to this loop
		}
		// Repeated start setup time, minimum 4.7us
		I2C_delay();
	}
	if (read_SDA() == 0) {
		arbitration_lost();
	}
	// SCL is high, set SDA from 1 to 0.
	clear_SDA();
	I2C_delay();
	clear_SCL();
	started = true;
}
 
void i2c_stop_cond(void) {
	// set SDA to 0
	clear_SDA();
	I2C_delay();
	// Clock stretching
	while (read_SCL() == 0) {
		// add timeout to this loop.
	}
	// Stop bit setup time, minimum 4us
	I2C_delay();
	// SCL is high, set SDA from 0 to 1
	if (read_SDA() == 0) {
		arbitration_lost();
	}
	I2C_delay();
	started = false;
}
 
// Write a bit to I2C bus
void i2c_write_bit(bool bit) {
	if (bit) {
		read_SDA();
	} else {
		clear_SDA();
	}
	I2C_delay();
	while (read_SCL() == 0) { // Clock stretching
		// You should add timeout to this loop
	}
	// SCL is high, now data is valid
	// If SDA is high, check that nobody else is driving SDA
	if (bit && read_SDA() == 0) {
		arbitration_lost();
	}
	I2C_delay();
	clear_SCL();
	I2C_delay();
}
 
// Read a bit from I2C bus
bool i2c_read_bit(void) {
	bool bit;
	// Let the slave drive data
	read_SDA();
	I2C_delay();
	while (read_SCL() == 0) { // Clock stretching
		// You should add timeout to this loop
	}
	// SCL is high, now data is valid
	bit = read_SDA();
	I2C_delay();
	clear_SCL();
	I2C_delay();
	return bit;
}
 
// Write a byte to I2C bus. Return 0 if ack by the slave.
bool i2c_write_byte(unsigned char byte) {
	unsigned bit;
	bool nack;
	for (bit = 0; bit < 8; bit++) {
		i2c_write_bit((byte & 0x80) != 0);
		byte <<= 1;
	}
	nack = i2c_read_bit();
	return nack;
}
 
// Read a byte from I2C bus
unsigned char i2c_read_byte(bool nack) {
	unsigned char byte = 0;
	unsigned bit;
	for (bit = 0; bit < 8; bit++) {
		byte = (byte << 1) | i2c_read_bit();
	}
	i2c_write_bit(nack);
	return byte;
}

//does a write, and if it fails it returns the function where it's called
#define i2c_write_byte_ex(byte) {if(i2c_write_byte(byte)) {i2c_stop_cond(); return 1;}}

//Writes a register, returns 0 if successful
bool i2c_write_register(unsigned char address, unsigned char reg, unsigned char data) {
	i2c_start_cond();
	printf("S");
	i2c_write_byte_ex(address+I2C_SLAVE_WRITE);
	printf("A");
	i2c_write_byte_ex(reg);
	printf("R");
	i2c_write_byte_ex(data);
	printf("D");
	i2c_stop_cond();
	return 0;
}

//Reads a register
unsigned char i2c_read_register(unsigned char address, unsigned char reg) {
	i2c_start_cond();
	i2c_write_byte(address+I2C_SLAVE_WRITE);
	i2c_write_byte(reg);
	i2c_start_cond();
	i2c_write_byte(address+I2C_SLAVE_READ);
	unsigned char data = i2c_read_byte(1);
	i2c_stop_cond();
	return data;
}