#ifndef OSCILLI
#define OSCILLI

#include "mbed.h"

void chip_select(uint8_t bit);
void init_arduino_comms(void);
void Read_Send_Signal(void);
void Send_Byte(uint8_t byte);
bool B1(bool write,bool val);
bool F1(bool write,bool val);
bool F2(bool write,bool val);
#endif