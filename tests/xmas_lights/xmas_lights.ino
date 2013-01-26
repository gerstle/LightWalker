#include <SPI.h>
#include <TCL.h>

const int LEDS = 50; // 50 LED Total Control Lighting Strand
const int COLORS = 6; // 5 colors and black.
const int BLACK = 0; // Define the colors for your code
const int RED = 1;
const int ORANGE = 2;
const int YELLOW = 3;
const int GREEN = 4;
const int BLUE = 5;

byte color_values[COLORS][3]; // This will store the RGB values of various colors
long change_time[LEDS]; // This array will store at what time the LEDs should change
int current_color[LEDS]; // This array stores an integer for each LED with its current color

void setup() {
  int i;
  unsigned long time;
  
  TCL.begin();
  
  // Set up RGB values for each color we have defined above
  color_values[BLACK][0] = 0x00;
  color_values[BLACK][1] = 0x00;
  color_values[BLACK][2] = 0x00;
  
  color_values[RED][0]=0xff;
  color_values[RED][1]=0x00;
  color_values[RED][2]=0x00;
  
  color_values[ORANGE][0]=0xff;
  color_values[ORANGE][1]=0x60;
  color_values[ORANGE][2]=0x00;
  
  color_values[YELLOW][0]=0xff;
  color_values[YELLOW][1]=0xb0;
  color_values[YELLOW][2]=0x00;
  
  color_values[GREEN][0]=0x00;
  color_values[GREEN][1]=0x80;
  color_values[GREEN][2]=0x00;
  
  color_values[BLUE][0]=0x00;
  color_values[BLUE][1]=0x00;
  color_values[BLUE][2]=0xff;

  time = millis(); // Find the current time
  for(i=0;i<LEDS;i++) {
    change_time[i] = time+random(700,1300); // Set up the time each LED should blink on in 700-1300 ms
    current_color[i]=BLACK; // Set each LED to black
  }
  update_strand(); // Update the colors along the strand of LEDs
}

void loop() {
  int i;
  unsigned long time;
  
  time=millis();
  
  for(i=0;i<LEDS;i++) {
    if(change_time[i]<time) {
      change_time[i]=time+random(700,1300);
      if(current_color[i]==BLACK) {
        current_color[i]=random(1,COLORS);
      }
      else {
        current_color[i]=BLACK;
      }
    }
  }
  update_strand();
}

void update_strand() {
  int i;
  int color;
  
  TCL.sendEmptyFrame();
  for(i=0;i<LEDS;i++) {
    color = current_color[i];
    TCL.sendColor(color_values[color][0],color_values[color][1],color_values[color][2]);
  }
  TCL.sendEmptyFrame();
}

