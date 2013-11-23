#pragma once

#include <stdio.h>
#include <string.h>
#include "USBSerial.h"

int getchar(void) {
	return 0;
}

int putchar(int c) {
	return USBSerial_write(c);
}