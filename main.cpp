#include "signal_generator.h"
#include "BLE.h"
#include "Oscilliscope.h"
#include "mbed.h"

Timer test;

int main() {
    initbt();
    init_arduino_comms();
    printf("boot\r\n");
    //define variable for this scope
    uint8_t frequency = 0;
    uint8_t wave = 0;
    uint8_t voltage = 0;
    uint8_t multiplier = 0;
    uint32_t change = 0;

    while(command==0){
        poll();
    }
    printf("%d\r\n",(command>>24)&0xFF);


    while(1){
        poll();
        if(command!=change){
            change = command;
            printf("wave = %X   voltage = %X   multiplier = %X   freq = %X   command = %X\r\n",wave,voltage,multiplier,frequency,command);
        }

        if(((command>>24)&0xFF)==1)
            Signal_Gen();
        if(((command>>24)&0xFF)==2){
            Read_Send_Signal();}

    }
}

