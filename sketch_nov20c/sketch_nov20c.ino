#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define GATE_PIN 2
#define SENSOR_BUTTON 8
#define STRIP_DATA 6

#define STRIPE_LENGTH 1

unsigned long last_millis = 0;

//int strip_color[3] = {50, 50, 50};

byte gammatable[256];


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPE_LENGTH, STRIP_DATA, NEO_GRB + NEO_KHZ800);

void setup() {
  // Setup Serial connection
  Serial.begin(9600);

  // Initialize strip
  strip.begin();
  strip.setBrightness(255);
  strip.show();

  // Setup pin mode
  pinMode(GATE_PIN, INPUT);
  pinMode(SENSOR_BUTTON, INPUT);
  pinMode(STRIP_DATA, OUTPUT);

  // Detect TCS
  if(tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("Sensor not found");
    while(1);
  }

  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if(digitalRead(GATE_PIN)) {
    light_up();
  } else {
    light_down();
  }
  if(millis() - last_millis > 1000) {
    if(digitalRead(SENSOR_BUTTON)) {
      change_color();
    }
    last_millis = millis();
  }
  */
  Serial.println("Red");
  strip.setPixelColor(0, strip.Color(255,0,0));
  strip.show();
  delay(1000);  
  Serial.println("Green");
  strip.setPixelColor(0, strip.Color(0,255,0));
  strip.show();
  delay(1000);
  Serial.println("Blue");

  strip.setPixelColor(0, strip.Color(0,0,255));
  strip.show();
  delay(1000);
  change_color();
  delay(5000);
}

void light_up() {
 
}

void light_down() {

}

void change_color() {
  tcs.setInterrupt(false);
  delay(60);
  uint16_t clear, red, green, blue;
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;

  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  Serial.print("Before: ");
  Serial.print(r);
  Serial.print(" ");
  Serial.print(g);
  Serial.print(" ");
  Serial.println(b);

  uint32_t color; 
  color = strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
  strip.setPixelColor(0,color);
  strip.show();

  delay(5000);

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
