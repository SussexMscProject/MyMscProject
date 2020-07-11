#include <MemoryFree.h>
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
const int b3 = 38;
const int f1 = 34;
const int f2 = 30;

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(f1, INPUT);
  pinMode(f2, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("boot");
  tft.reset();
  tft.begin(37697);
  tft.setRotation(1);
}

const uint16_t width = 320;
const uint8_t height = 240;

void loop(void){
  if(digitalRead(b1)==HIGH)
    print_wave();
  if(digitalRead(b3)==HIGH)
    Signal_gen();
  Serial.println("im in the main loop");
}

void Signal_gen(){

  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.println("Generating Signal");
  while(digitalRead(b3)==HIGH){
  }
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);  
  tft.println("Generating Signal:");
}
uint8_t pixel_buffer[320][2];
void print_wave(){
  uint8_t vmin = 255;
  uint8_t vmin_ref = 255;
  uint8_t vmax = 0;
  uint8_t vmax_ref;
  uint16_t i = 0;
  uint8_t threshold = 0;
  uint16_t crossed = 0;
  uint8_t sample_split = 0;
  uint16_t volt_split = 0;
  uint16_t volt_split_ref = 0;
  uint32_t frequency = 0;
  uint32_t frequency_ref = 0;
  uint16_t time_div = 0;
  uint16_t time_div_ref = 0;
  uint32_t volt_div_ref = 0;
  uint32_t volt_div = 0;
  tft.setTextSize(1);
  tft.fillScreen(BLACK);
  draw_text();
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

    volt_div = (3300/6)*(100.0/float(volt_split));
    if(volt_div!=volt_div_ref){
      update_text(100, 10, volt_div_ref, volt_div);
      volt_div_ref = volt_div;
    }
    
    frequency = (1000000L*crossed)/(10L*sample_split*32);
    if(frequency!=frequency_ref){
      update_text(100, 20, frequency_ref, frequency);
      frequency_ref = frequency;
    }
    
    if(vmax!=vmax_ref){
      update_text(100, 30, (long(vmax_ref)*1000L)/3300L, (long(vmax)*1000L)/3300L);
      vmax_ref = vmax;
    }
  }
}

void update_text(uint32_t x, uint32_t y, uint32_t ref, uint32_t val){
  
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