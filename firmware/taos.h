#pragma once

#include "i2c.h"

unsigned char TAOS_address=0x29<<1;

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