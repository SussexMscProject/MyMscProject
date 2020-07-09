#include "BLE.h"

uint8_t func = 0;
uint32_t command;
RawSerial device(PC_12, PD_2);
volatile bool global = 0;

void initbt(void){
    device.attach(&callback);
}

void callback(){
    device.attach(NULL);
    global = 1;
}

uint32_t poll(){
    if (global) {
        command = BT();
        global = 0;
        device.attach(&callback);
    }
    return command;
}

void flushSerialBuffer(void){ 
    char char1 = 0; 
    while (device.readable()){ 
        char1 = device.getc();
    } 
    return;
}

uint32_t BT(void){
    char        read[4];
    uint32_t    hold = 0;
    int i = 0;
    if(device.readable()){
        while(i<4){
            while(device.readable()){
                read[i] = device.getc();
                i++;
            }
        }
        hold = (read[3] << 24) + (read[2] << 16) + (read[1] << 8) + read[0];
        return hold;
        flushSerialBuffer();
    }
    return command;
}

