
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define analogPin      13          // analog pin for measuring capacitor voltage
#define chargePin      22         // pin to charge the capacitor - connected to one end of the charging resistor
#define dischargePin   25         // pin to discharge the capacitor
#define resistorValue  10000.0F   // change this to whatever resistor value you are using

const int b1 = 36;
const int b2 = 23;
const int b3 = 38;
const int b4 = 24;
const int f1 = 34;
const int f2 = 30;

#define vin 5.0

long R1[] = {220,470,670,1000,10000,22000,47000,74000,100000,470000};
int pins[] = {40,42,44,46,48,50,52,30,28,26};
static const uint8_t analog_R = A12;
int output_impedance = 740;

/*
#define k22 220 //40
#define k47 470 //42
#define k67 670  //44
#define 1k 1000 //46
#define 10k 10000 //48
#define 22k 22000 //50
#define 47k 47000 //52
#define 74k 74000 //51
#define 100k 100000 //49
#define 470k 470000 //47
*/

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  
  //analog pin
  pinMode(analog_R, INPUT);

  //digital pins
  for(int i = 0;i<10;i++){  
    pinMode(pins[i], INPUT);
  }
  
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  pinMode(f1, INPUT);
  pinMode(f2, OUTPUT);
  
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("boot");
  
  pinMode(chargePin, OUTPUT);     // set chargePin to output
  digitalWrite(chargePin, LOW);  
  
  tft.reset();
  tft.begin(37697);
  tft.setRotation(1);
}

const uint16_t width = 320;
const uint16_t height = 240;

void loop(void){
    //depending on what pin the nucleo had set high
  if(digitalRead(b1)==HIGH)
    print_wave();
  if(digitalRead(b2)==HIGH)
    ohmmeter();
  if(digitalRead(b3)==HIGH)
    Signal_gen();
  if(digitalRead(b4)==HIGH)
    capacitance();
  tft.fillScreen(BLACK);
}

void capacitance(){
  tft.fillScreen(BLACK);
  unsigned long startTime;
  unsigned long elapsedTime;
  unsigned long Time;
  float microFarads;                // floating point variable to preserve precision, make calculations
  float nanoFarads;
  long Farads;
  while(digitalRead(b4)==HIGH){
    //start charging
    digitalWrite(chargePin, HIGH);  // set chargePin HIGH and capacitor charging
    //record current time
    startTime = millis();
    //while analog voltage is < 648 out of 1023 charging
    while(analogRead(analogPin) < 648){}
    //record difference in time
    elapsedTime= millis() - startTime;
   // convert milliseconds to seconds ( 10^-3 ) and Farads to microFarads ( 10^6 ),  net 10^3 (1000) 
   //lcd printing
    tft.setTextSize(2);
    
    tft.setTextColor(BLACK);
    tft.setCursor(0, 30);  
    tft.print(Time);  
    tft.setTextColor(RED);
    tft.setCursor(0, 30);  
    tft.print(elapsedTime);     
    tft.setTextColor(WHITE);       
    tft.print(" mS");   
    Time = elapsedTime;      
    //calculate micro farads
    microFarads = ((float)elapsedTime / resistorValue) * 1000;  
    if (microFarads > 1){
      tft.setCursor(0, 0);   
      tft.setTextColor(BLACK);
      tft.print(Farads); 
      tft.setCursor(0, 0);   
      tft.setTextColor(RED);
      tft.print((long)microFarads);   
      tft.setTextColor(WHITE);    
      tft.println(" microFarads");   
      Farads = (long)microFarads;
    }
    //if less than one calculate nanofarads
    else{
      tft.setCursor(0, 0);   
      tft.setTextColor(BLACK);
      tft.print(Farads); 
      tft.setCursor(0, 0);   
      tft.setTextColor(RED);
      //factor by 1000
      nanoFarads = microFarads * 1000.0;      
      tft.print((long)nanoFarads);     
      tft.setTextColor(WHITE);            
      tft.println(" nanoFarads");    
      Farads = (long)nanoFarads;   
    }
    //discharge capacitor
    digitalWrite(chargePin, LOW);             
    pinMode(dischargePin, OUTPUT);           
    digitalWrite(dischargePin, LOW);          
    while(analogRead(analogPin) > 0){         
    }
  
    pinMode(dischargePin, INPUT);
  }
}

void ohmmeter(){
    unsigned long sensorValue[10];
    int samples = 50;
    uint16_t sensorVal_ref = 0;
    uint16_t R1_ref = 0;
    float vref = 0;
    float cref;
    while(digitalRead(b2)==HIGH){
        //set best value to max 10 bit val
      int BestValue = 1024;
      int pointer = 0;
      //loop for each resistor
      for(int i = 0;i<10;i++){  
        //set to output
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i],HIGH);
        //reset sensor val
        sensorValue[i]=0;
        analogRead(analog_R);
        //take average
        for(int j = 0;j<samples;j++){
          sensorValue[i]+=analogRead(analog_R);
        }
        sensorValue[i]/=samples;
        digitalWrite(pins[i],LOW);
        //set for high impedence
        pinMode(pins[i], INPUT);
        //debug
        Serial.println(sensorValue[i]);
      }
      //calculate the resistor that is closest to midpoint 1024/2
      for(int j = 0;j<10;j++){
        if(abs(512-sensorValue[j])<BestValue){
          BestValue = abs(512-sensorValue[j]);
          pointer = j;
        }
      }
      
      tft.setTextSize(2);
      //lcd printing
      if(sensorValue[pointer]!=sensorVal_ref){
        tft.fillScreen(BLACK);
        tft.setTextColor(WHITE);
        tft.setCursor(0, 0);  
        tft.print("Sensor value:");
        tft.setCursor(0, 20);
        tft.setTextColor(BLACK);
        tft.print(sensorVal_ref);
        tft.setCursor(0, 20);
        tft.setTextColor(RED);
        tft.print(sensorValue[pointer]);
        tft.setTextColor(WHITE);  
        tft.print("/1023");
        sensorVal_ref = sensorValue[pointer];
      //}
      //if(R1_ref!=R1[pointer]){
        tft.setTextColor(WHITE);
        tft.setCursor(0, 40);  
        tft.print("Known Resistor value:");
        tft.setCursor(0, 65);
        tft.setTextColor(BLACK);
        tft.print(R1_ref);
        tft.setCursor(0, 65);
        tft.setTextColor(RED);
        tft.print(R1[pointer]);
        tft.setTextColor(WHITE);  
        tft.print(" ohms");
        R1_ref=R1[pointer];
        
      //}
      
      float voltage=(sensorValue[pointer]*vin)/1024.0;
      //if(vref!=voltage){ 
        tft.setTextColor(WHITE);
        tft.setCursor(0, 85);  
        tft.println("voltage:");
        tft.setCursor(0, 110);
        tft.setTextColor(BLACK);
        tft.println(vref);
        tft.setCursor(0, 110);
        tft.setTextColor(RED);  
        tft.print(voltage);
        tft.setTextColor(WHITE);  
        tft.print(" Volts");
        
        tft.setCursor(0, 130);
        tft.setTextColor(WHITE);  
        tft.print("Resistor value is: ");
        tft.setCursor(0, 155);
        tft.setTextColor(BLACK);  
        tft.print(String(resistance(vref,pointer)));
        tft.setCursor(0, 155);
        tft.setTextColor(RED);  
        tft.print(String(resistance(voltage,pointer)));
        tft.setTextColor(WHITE);  
        tft.print(" ohms");
        
        vref = voltage;
  
      //}
      //v/r = i
      float current=voltage/resistance(voltage,pointer);
      current*=1000.0;
      //if(cref!=current){ 
        tft.setTextColor(WHITE);
        tft.setCursor(0, 175);  
        tft.println("Current:");
        tft.setCursor(0, 200);
        tft.setTextColor(BLACK);
        tft.println(cref);
        tft.setCursor(0, 200);
        tft.setTextColor(RED);  
        tft.print(current);
        tft.setTextColor(WHITE);  
        tft.print(" mA");
        
        cref = current;
  
      }
      delay(3000);
    }
}

unsigned long resistance(float vout,int pointer){
    //calculate resistance
  return (R1[pointer]+output_impedance)*(1/(vin/vout-1));
}

void screen(String text,uint16_t color){
    
    tft.setTextColor(WHITE);  
    tft.println("Resistor value is: ");
    tft.setTextColor(RED);  
    tft.print(text);
    tft.setTextColor(WHITE);  
    tft.println(" ohms");
  
}

void Signal_gen(){

  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.println("Generating Signal");
  uint8_t frequency;
  uint8_t voltage;
  digitalWrite(f2,HIGH);
  uint8_t buffer_[3][2];
  uint8_t i = 0;
  while(digitalRead(b3)==HIGH){
      //wait for Nucleo to be ready
    while((digitalRead(f1)==LOW)&&(digitalRead(b3)==HIGH)){

    }
    //read from transmission buffer
    if(Serial1.available()){
      while(i<3){
        buffer_[i][1] = Serial1.read();
        i++;
      }
      i = 0;
    }

    if(buffer_[2][1]!=buffer_[2][2]){
      String wave;
      String old_wave;
      Serial.println("helo worlld");
      switch(buffer_[3][1]){
        case 0:
          wave = "SawTooth";
          break;
        case 1:
          wave = "Square";
          break;
        case 2:
          wave = "Sine";
          break;
        case 3:
          wave = "DC";
          break;
        buffer_[3][2]=buffer_[3][1];
      }
      tft.setCursor(0, 30);
      tft.setTextColor(BLACK);  
      tft.println(old_wave);
      tft.setCursor(0, 30);
      tft.setTextColor(WHITE);  
      tft.println(wave);
    }
  }
  digitalWrite(f2,LOW);
}

uint16_t pixel_buffer[320][2];
void print_wave(){
  uint16_t vmin = 255;
  uint16_t vmin_ref = 255;
  uint16_t vmax = 0;
  uint16_t vmax_ref;
  uint16_t i = 0;
  uint16_t threshold = 0;
  uint16_t crossed = 0;
  uint16_t sample_split = 0;
  uint16_t volt_split = 1;
  uint16_t volt_split_ref = 1;
  float frequency = 0;
  float frequency_ref = 0;
  float time_div = 0;
  float time_div_ref = 0;
  char buff1[10];
  char buff2[10];
  float volt_div_ref = 0;
  float volt_div = 0;
  tft.setTextSize(1);
  tft.fillScreen(BLACK);
  draw_text();
  //flush serial buffer
  Serial1.flush();
  while(digitalRead(b1)==HIGH){
    i = 0;
    digitalWrite(f2,HIGH);
    //wait for nucleo to be ready
    while((digitalRead(f1)==LOW)&&(digitalRead(b1)==HIGH)){}
    //all is in sync so read serial
    while((i<320)&&(digitalRead(b1)==HIGH)){
        //take 320 samples from nucleo
      if((Serial1.available() > 0)&&(digitalRead(b1)==HIGH)){
        pixel_buffer[i][2] = Serial1.read();
        i++;
      }
    }
    //Serial.println(" ");
    //Serial.println(pixel_buffer[318][2]);
    //Serial.println(pixel_buffer[319][2]);
    //signal that all message has been read
    digitalWrite(f2,LOW);
    sample_split = pixel_buffer[318][2]+15;
    volt_split = pixel_buffer[319][2];
    i = 0;
    vmin = 255;
    vmax = 0;
    //find smallest and largest values for vmax and vmin
    while((i<318)&&(digitalRead(b1)==HIGH)){
      if (vmax < pixel_buffer[i][2])
        vmax = pixel_buffer[i][2];
      if (vmin > pixel_buffer[i][2])
        vmin = pixel_buffer[i][2];
      i++;
    }
    //calculate halfway point
    threshold = (vmax-vmin)/2-vmin;
    crossed = 0;
    i = 0;
    //calculate how many time the threshold is crassed to find frequency
    while((i<318)&&(digitalRead(b1)==HIGH)){
      if(((pixel_buffer[i][2])<threshold)&&((pixel_buffer[i-1][2])>threshold))
        crossed++;
      i++;
    }
    i = 0;
    
    //Serial.println(sample_split);
    //Serial.println(volt_split);
    
    i=1;
    //print last wave in black to erase
    while((i<318)&&(digitalRead(b1)==HIGH)){
      tft.drawLine(i-1, 240-(float(pixel_buffer[i-1][1])*0.7*volt_split_ref/255.0),i , 240-(float(pixel_buffer[i][1])*0.7*volt_split_ref/255.0),BLACK);
      i++;
    }
    //redraw grid
    draw_grid();
    i=1;
    //print new wave in yellow
    while((i<318)&&(digitalRead(b1)==HIGH)){
      tft.drawLine(i-1, 240-(float(pixel_buffer[i-1][2])*0.7*volt_split/255.0),i, 240-(float(pixel_buffer[i][2])*0.7*volt_split/255.0),YELLOW);
      pixel_buffer[i][1]=pixel_buffer[i][2];
      volt_split_ref = volt_split;
      i++;
    }
    i = 0;
    
    if(volt_split!=volt_split_ref)volt_split_ref=volt_split;
    //print time of grid box
    time_div = sample_split*32;
    if(time_div!=time_div_ref){
      update_text(100, 0, dtostrf(time_div_ref, 5,2, buff1), dtostrf(time_div, 5,2, buff2));
      time_div_ref = time_div;
    }

    //print voltage of grid box
    volt_div = (3300.0/6.0)*(255.0/float(volt_split));
    if(volt_div!=volt_div_ref){
      update_text(100, 10, dtostrf(volt_div_ref, 5,2, buff1), dtostrf(volt_div, 5,2, buff2));
      volt_div_ref = volt_div;
    }
    //caluclate frequency depending in sample rate threshold crost and pixel width
    frequency = (1000000.0/(float(sample_split)*318.0/float(crossed)));
    Serial.println(" ");
    Serial.println(frequency);
    Serial.println(float(sample_split));
    Serial.println(crossed);
    //print frequency
    if(frequency!=frequency_ref){
      update_text(100, 20, dtostrf(frequency_ref, 5,2, buff1), dtostrf(frequency, 5,2, buff2));
      frequency_ref = frequency;
    }
    //print v max
    if(vmax!=vmax_ref){
      update_text(100, 30, dtostrf((long(vmax_ref*2)*1000L)/3300L, 5,2, buff2), dtostrf((long(vmax*2)*1000L)/3300L, 5,2, buff2));
      vmax_ref = vmax;
    }
    //print v min
    if(vmin!=vmin_ref){
      update_text(100, 40, dtostrf((long(vmin_ref*2)*1000L)/3300L, 5,2, buff2), dtostrf((long(vmin*2)*1000L)/3300L, 5,2, buff2));
      vmin_ref = vmin;
    }
  }
}

//function to write last value in black and new value in white
void update_text(uint32_t x, uint32_t y, String ref, String val){
  tft.setCursor(x, y);
  tft.setTextColor(BLACK);
  tft.println(ref);
  tft.setTextColor(WHITE);  
  tft.setCursor(x, y);
  tft.println(val);
}


void draw_text(){
  tft.setTextColor(WHITE);  
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.println("Time div/us");
  tft.setCursor(0, 10);  
  tft.setTextSize(1);
  tft.println("Volt div/mV");
  tft.setCursor(0, 20);  
  tft.setTextSize(1);
  tft.println("Frequency/Hz");
  tft.setCursor(0, 30);  
  tft.setTextSize(1);
  tft.println("Volt max/mV");
  tft.setCursor(0, 40);  
  tft.setTextSize(1);
  tft.println("Volt min/mV");
}

//draws red grid for oscilliscope
void draw_grid(){
                            tft.drawFastVLine(0, 60, height, RED);
                            tft.drawFastVLine(width-1, 60, height-40, RED);
  for(int i = 1; i<10; i++){tft.drawFastVLine(i*32, 60, height-40, RED);}
  for(int i = 1; i<65; i++){tft.drawFastVLine(i*5, 60+90-3, 7, RED);}
  for(int i = 1; i<36; i++){tft.drawFastHLine(160-3, i*5+60, 7, RED);}
                            tft.drawFastHLine(0,60, width, RED);
  for(int i = 1; i<6; i++){ tft.drawFastHLine(0,60+i*30, width, RED);}
                            tft.drawFastHLine(0, height-1, width, RED);
}
