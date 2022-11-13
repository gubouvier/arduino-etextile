#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define PIN_GATE_IN 2
#define PIN_LED_OUT 13

#define STRIPE_LENGTH 1
#define PIN_STRIPE 6

#define PIN_SETUP_COLOR 3
#define PIN_SET_WHITE 4

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPE_LENGTH, PIN_STRIPE, NEO_GRB + NEO_KHZ800);

byte gammatable[256];

void setup() {
  // Setup serial
  Serial.begin(9600);

  // Initialize strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Detect TCS
  // TODO: replace with 'while not tcs.begin()'
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // Setup pinMode
  pinMode(PIN_LED_OUT, OUTPUT);
  pinMode(PIN_GATE_IN, INPUT);

  // Setup Interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_GATE_IN), ISR_sound_beat, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_SETUP_COLOR), ISR_set_stripe_color, RISING);


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
  if(digitalRead(PIN_SET_WHITE)) {
        set_stripe_white();
  }
  delay(500);
}

void ISR_set_stripe_color(void) {
  tcs.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  uint16_t clear, red, green, blue;

  tcs.getRawData(&red, &green, &blue, &clear);

  tcs.setInterrupt(true);  // turn off LED

  tcs.getRawData(&red, &green, &blue, &clear);

  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  //sum += clear; // clear contains RGB already so no need to re-add it
  
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  uint32_t color = strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
  colorWipe(color, 0);
}

void ISR_sound_beat(void) {
  int pin_val;

  pin_val = digitalRead(PIN_GATE_IN);
  digitalWrite(PIN_LED_OUT, pin_val);
}

void set_stripe_white(void) {
  uint32_t white = strip.Color(0,0,0,255);
  colorWipe(white, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}