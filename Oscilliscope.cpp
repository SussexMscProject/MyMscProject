#include "Oscilliscope.h"
#include "BLE.h"

AnalogIn ain(A0);
DigitalOut b1(D1);
DigitalOut f1(D9);
DigitalIn f2(D13);
Serial TXRX(PD_5, PD_6); //TX, RX
Timer t2;

bool F2(){
    return f2;
}

void F1(bool write){
    if(write)
        f1 = 1;
    else
        f1 = 0;
}

void B1(bool write){
    if(write)
        b1 = 1;
    else
        b1 = 0;
}

void init_arduino_comms(void){
    TXRX.baud(115200);
}

void Read_Send_Signal(void){
    IWDG_HandleTypeDef hiwdg;
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256; 
    hiwdg.Init.Reload = 1000; 
    b1 = 1;
    uint16_t i = 0;
    uint16_t buffersize = 318;
    uint16_t _time = 15;
    char samples[buffersize+2];
    t2.start();
    printf("line 23 %d\r\n",((poll()>>24)&3)==2);
    while((((poll()>>24)&3)==2)){
        _time = 15+poll()&0xFF;
        i = 0;
        while((((poll()>>24)&3)==2)&&(i<buffersize)){
            if(t2.read_us()>_time){
                samples[i] = char(ain.read()*255*2);
                i++;
                t2.reset(); 
            }
        }
        i = 0;
        f1 = 1;
        while((f2==0)&&((poll()>>24)&3)){}
        samples[318]=poll()&0xFF;
        samples[319]=(poll()>>8)&0xFF;
        while((((poll()>>24)&3)==2)&&(i<buffersize+2)){
            TXRX.putc(samples[i]);
            i++;
        }
        HAL_IWDG_Init(&hiwdg);
        f1 = 0;
    }
    b1 = 0;
    t2.stop();
    t2.reset();
}

void Send_Byte(uint8_t byte){
    TXRX.putc(byte);
}
