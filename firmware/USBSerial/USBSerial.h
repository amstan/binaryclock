#ifndef USBSerial_h
#define USBSerial_h

#include <stdint.h>

void USBSerial_open();
void USBSerial_close();
int USBSerial_read();
int USBSerial_available();
uint16_t USBSerial_write(char b);
uint16_t USBSerial_write(const char *buffer, uint16_t size);
void USBSerial_flush();
int USBSerial_peek();

#endif

