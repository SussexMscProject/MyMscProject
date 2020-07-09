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

const int b1 = 36;
const int b2 = 50;
const int b3 = 48;
const int b4 = 46;
const int b5 = 44;
const int b6 = 42;
const int b7 = 40;
const int b8 = 38;
const int f1 = 34;
const int f2 = 30;

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  pinMode(b5, INPUT);
  pinMode(b6, INPUT);
  pinMode(b7, INPUT);
  pinMode(b8, INPUT);
  pinMode(f1, INPUT);
  pinMode(f2, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("boot");
  tft.reset();
  tft.begin(37697);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  draw_text();
}

const uint16_t width = 320;
const uint8_t height = 240;

void loop(void){
  draw_grid();
  if(digitalRead(b1)==HIGH)
    print_wave();
  Serial.println("im in the main loop");
}

void print_wave(){
  
  uint8_t vmin = 255;
  uint8_t vmax = 0;
  uint8_t pixel_buffer[320][2];
  uint16_t i = 0;
  uint8_t threshold = 0;
  uint16_t crossed = 0;
  uint8_t sample_split = 0;
  uint16_t volt_split = 0;
  uint16_t volt_split_ref = volt_split;
  uint32_t frequency = 0;
  uint32_t frequency_ref = frequency;
  uint16_t time_div = 0;
  uint16_t time_div_ref = 0;
  tft.setTextSize(1);
  Serial1.flush();
  while(digitalRead(b1)==HIGH){
    i = 0;
    digitalWrite(f2,HIGH);
    while((digitalRead(f1)==LOW)&&(digitalRead(b1)==HIGH)){}
    while((i<320)&&(digitalRead(b1)==HIGH)){
      if((Serial1.available() > 0)&&(digitalRead(b1)==HIGH)){
        pixel_buffer[i][2] = Serial1.read();
        i++;
      }
    }
    digitalWrite(f2,LOW);
    i = 0;
    vmin = 255;
    vmax = 0;
    while((i<318)&&(digitalRead(b1)==HIGH)){
      if (vmax < pixel_buffer[i][2])
        vmax = pixel_buffer[i][2];
      if (vmin > pixel_buffer[i][2])
        vmin = pixel_buffer[i][2];
      i++;
    }

    threshold = (vmax-vmin)/2-vmin;
    crossed = 0;
    i = 0;
    while((i<320)&&(digitalRead(b1)==HIGH)){
      if(((pixel_buffer[i][2])<threshold)&&((pixel_buffer[i-1][2])>threshold))
        crossed++;
      i++;
    }
    i = 0;
    sample_split = pixel_buffer[318][2];
    volt_split = pixel_buffer[319][2];
     
    draw_grid();
    i=1;
    while((i<318)&&(digitalRead(b1)==HIGH)){
      tft.drawFastVLine(i, 240-pixel_buffer[i][1]*0.7*(float(volt_split_ref)/100.0), 1, BLACK);
      tft.drawFastVLine(i, 240-pixel_buffer[i][2]*0.7*(float(volt_split)/100.0), 1, YELLOW);
      pixel_buffer[i][1] = pixel_buffer[i][2];
      i++;
    }
    i = 0;
    
    if(volt_split!=volt_split_ref)volt_split_ref=volt_split;
    
    time_div = 10*sample_split*32;
    if(time_div!=time_div_ref){
      update_text(100, 0, time_div_ref, time_div);
      time_div_ref = time_div;
    }
    
    //frequency = (((10*sample_split*320)/(crossed*1000000)));
    Serial.println((((10*sample_split*320)/(crossed*1000000))));
//    if(frequency!=frequency_ref){
//      update_text(100, 20, time_div_ref, short(frequency));
//      frequency_ref = frequency;
//    }
    //Serial.println(frequency);
    //canary();
    
  }
}

void canary(){
    Serial.print(b2);
    Serial.print(" ");
    Serial.print(b3);
    Serial.print(" ");
    Serial.print(b4);
    Serial.print(" ");
    Serial.print(b5);
    Serial.print(" ");
    Serial.print(b6);
    Serial.print(" ");
    Serial.println(b7);
  
}

void update_text(uint8_t x, uint8_t y, uint16_t ref, uint16_t val){
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
  tft.println("Volt div");
  tft.setCursor(0, 20);  
  tft.setTextSize(1);
  tft.println("Frequency");
  tft.setCursor(0, 30);  
  tft.setTextSize(1);
  tft.println("Volt max");
  tft.setCursor(0, 40);  
  tft.setTextSize(1);
  tft.println("Volt min");
}


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
