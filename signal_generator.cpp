#include "signal_generator.h"
#include "BLE.h"

AnalogOut aout(PA_4);
Timer t;

void Signal_Gen(){
    uint8_t wave = (poll()>>16)&0xFF;
    //sawtooth
    if(wave==0)
        SawTooth();
    //square
    if(wave==1)
        Square();
    //sine wave
    if(wave==2)
        Sine();
    //DC
    if(wave==3)
        DC();
}

void Sine(){

    uint8_t  sine_wave[256] = {
    0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
    0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
    0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
    0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
    0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
    0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
    0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
    0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
    0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
    0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
    0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
    0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
    0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
    0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
    0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
    0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
    0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
    0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
    0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
    0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
    0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
    0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
    0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
    0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
    0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
    0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
    0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
    0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
    0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
    };

    uint8_t i = 0;
    uint8_t frequency = 1;
    bool dir = 1;
    bool loop = 1;
    float voltage = 1;
    float _time = 1000/(frequency);
    float output;
    t.start();

    uint32_t message;
    printf("%d\r\n",_time);
    while(loop){

        i+=6;       
        aout = float(sine_wave[i])/float(0xFF)*0.9*voltage/100;
        message = poll();
        frequency = message&0xFF;
        voltage = (message>>8)&0xFF;

        _time = 1000/(frequency);
        if(((message>>16)&3)!=2)
            loop=0;

        while(t.read_us()<_time){}
        t.reset();
    }
    t.reset();
    t.stop();
}

void SawTooth(void){
    printf("%d\r\n",(poll()>>16)&0xFF);
    float i = 0;
    float voltage = 1;
    uint32_t message;
    uint8_t frequency = 1;
    float _time = 1000/(frequency);
    t.start();
    while(((poll()>>16)&0xFF)==0){
        if(i>=1)
            i=0;
        //printf("%f   %f\r\n",i,float(i*0.8*voltage/100));
        i+=0.01;
        aout = float(i*0.9*voltage/100);
        message = poll();
        frequency = message&0xFF;
        voltage = (message>>8)&0xFF;
        _time = 1000/(frequency);
        while(t.read_us()<_time){}
        t.reset();
    }
    t.stop();
    printf("ive changed\r\n");
}

void Square(void){
    printf("%d\r\n",(poll()>>16)&0xFF);
    float i = 0;
    float voltage = 1;
    uint32_t message;
    uint8_t frequency = 1;
    float _time = 1000/(frequency);
    bool dir = 0;
    t.start();
    while(((poll()>>16)&0xFF)==1){
        
        if(i<=0)
            dir = 0;
        if(i>=100)
            dir = 1;
        if(dir)
            i--;
        if(!dir)
            i++;

        aout = float(dir*voltage/100);

        message = poll();
        frequency = message&0xFF;
        voltage = (message>>8)&0xFF;
        _time = 1000/(frequency);
        while(t.read_us()<_time){}
        t.reset();
    }
    t.stop();
}

void DC(void){
    printf("%d\r\n",(poll()>>16)&0xFF);
    float voltage = 1;
    float _time = 10000;
    uint32_t message;
    t.start();
    while(((poll()>>16)&0xFF)==3){
        
        aout = float(voltage/100);
        message = poll();
        voltage = (message>>8)&0xFF;
        while(t.read_us()<_time){}
        t.reset();
    }
    t.stop();
}




