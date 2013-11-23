#pragma once

#define G 0
#define R 1
#define B 2

extern "C" {
	void write_ws2811_hs(uint8_t *data, uint16_t length, uint8_t pinmask);
}