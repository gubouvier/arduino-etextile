#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define GATE_PIN 2
#define SENSOR_BUTTON 8
#define MODE_BUTTON 9
#define STRIP_DATA 6

#define STRIPE_LENGTH 4

unsigned long last_millis = 0;

byte gammatable[256];

// Light mode, 0 = off, 1 = beat (reset to white), 2 = steady 
int mode = 2;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPE_LENGTH, STRIP_DATA, NEO_GRB + NEO_KHZ800);

uint32_t strip_color = strip.Color(255,255,255);


void setup() {
  // Setup Serial connection
  Serial.begin(9600);

  // Setup pin mode
  pinMode(GATE_PIN, INPUT);
  pinMode(SENSOR_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(STRIP_DATA, OUTPUT);

  // Initialize strip
  strip.begin();
  strip.setBrightness(255);
  strip.clear();
  strip.show();
/*
  // Detect TCS
  if(tcs.begin()) {
    Serial.println("Found sensor");
    tcs.setInterrupt(true);
  } else {
    Serial.println("Sensor not found");
    while(1);
  }
*/
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }

  init_test();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(mode == 0) {
    light_down();   
  } else if(digitalRead(GATE_PIN) == HIGH && mode == 1) {
    light_up();
  } else if (mode ==1 ) {
    light_down();
  } else if (mode == 2) {
    light_up();
  }

  if(millis() - last_millis > 1000) {
    if(digitalRead(SENSOR_BUTTON) == LOW) {
      change_color();
    }
    if(digitalRead(MODE_BUTTON) == LOW) {
      change_mode();
    }
    last_millis = millis();
  }
}

void light_up() {
  Serial.println("UP");
  for (int i = 0; i < STRIPE_LENGTH; i++) {
    strip.setPixelColor(i, strip_color);
  }
  strip.show();
  
}

void light_down() {
  Serial.println("DOWN");
  for (int i = 0; i < STRIPE_LENGTH; i++) {
    strip.clear();
  }
  strip.show();
}

void change_color() {
  tcs.setInterrupt(false);
  delay(1000);
  uint16_t clear, red, green, blue;
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;

  uint32_t color; 

  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  increase_brightness(&r, &g, &b);

  Serial.print("After: ");
  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);
  
  color = strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
  strip.setPixelColor(0,color);
  strip.show();
  delay(1000);
  strip_color = color;

}

void increase_brightness(float *r, float *g, float *b) {
  float max_value = find_max(*r, *g, *b);
  float multiplier = 255 / max_value;
  *r *= multiplier;
  *g *= multiplier;
  *b *= multiplier;
}

float find_max(float r, float g, float b) {
  float array[3] = {r, g, b};
  int max = 0;
  for(int i=1; i<3; i++) {
    if(array[i] > array[max]) {
      max = i;
    }
  }
  return array[max];
}

void change_mode(){
  if(mode == 0) {
    mode++;
    strip_color = strip.Color(255, 255, 255);
  }
  else if(mode ==1) {
    mode++;
  }
  else {
    mode = 0;
  }
  blink_blue();
}
  
void blink_blue() {
  for (int i=0; i<2; i++) {
    for (int j = 0; j < STRIPE_LENGTH; j++) {
      strip.setPixelColor(j, strip.Color(0, 0, 255));
    }
    strip.show();
    delay(500);
    strip.clear();
    strip.show();
    delay(500);
  }
}

void init_test(){
  uint32_t colors[4] = {strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255), strip.Color(255, 255, 255)};
  for(int i = 0; i < STRIPE_LENGTH; i++) {
    strip.setPixelColor(i, colors[i]);
    strip.show();
    delay(500);
  }
}
