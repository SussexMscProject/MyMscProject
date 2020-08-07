#include "signal_generator.h"
#include "BLE.h"
#include "Oscilliscope.h"
#include "mbed.h"

Timer test;
DigitalOut test_R(D15);
DigitalOut test_C(D14);

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

    
    IWDG_HandleTypeDef hiwdg;
    
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256; 
    hiwdg.Init.Reload = 1000;  
    //watchdog refresh rate at (256*1000)/(32*10^3)  = 8 seconds   

    while(command==0){         
        HAL_IWDG_Init(&hiwdg);
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
        while(((poll()>>24)&0xFF)==3){
            HAL_IWDG_Init(&hiwdg);
            test_R = 1;
        }
        if(test_R == 1)
            test_R = 0;
        while(((poll()>>24)&0xFF)==4){          
            HAL_IWDG_Init(&hiwdg);
            test_C = 1;
        }
        if(test_C == 1)          
            test_C = 0;
        
    }
}
