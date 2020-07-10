#include "Oscilliscope.h"
#include "BLE.h"

AnalogIn ain(A0);
DigitalOut b1(D1);
DigitalOut f1(D9);
DigitalIn f2(D13);
Serial TXRX(PD_5, PD_6); //TX, RX
Timer t2;

void init_arduino_comms(void){
    TXRX.baud(115200);
}

void Read_Send_Signal(void){
    b1 = 1;
    uint16_t i = 0;
    uint16_t buffersize = 318;
    uint16_t _time = 1;
    char samples[buffersize];
    t2.start();
    printf("line 23 %d\r\n",((poll()>>24)&3)==2);
    while((((poll()>>24)&3)==2)){
        if((poll()&0xFF)>1)
            _time = 100*(poll()&0xFF);
        i = 0;
        while((((poll()>>24)&3)==2)&&(i<buffersize)){
            if(t2.read_us()>_time){
                samples[i] = char(ain.read()*255);
                i++;
                t2.reset(); 
            }
        }

        i = 0;
        f1 = 1;
        while(f2==0){}
        while((((poll()>>24)&3)==2)&&(i<buffersize)){
            TXRX.putc(samples[i]);
            i++;
        }
        printf("%d    %d\r\n",poll()&0xFF,(poll()>>8)&0xFF);
        TXRX.putc(poll()&0xFF);
        //printf("%d\r\n",(poll()>>8)&0xFF);
        TXRX.putc(char((poll()>>8)&0xFF));
        f1 = 0;
        printf("poll()>>24)&3)==%d\r\n",(poll()>>24)&3);
    }
    printf("i escaped\r\n");
    b1 = 0;
    t2.stop();
    t2.reset();
}

void Send_Byte(uint8_t byte){
    TXRX.putc(byte);
}

