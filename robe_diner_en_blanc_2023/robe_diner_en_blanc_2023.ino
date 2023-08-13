#include <Adafruit_NeoPixel.h>

#define LED_PIN 1
#define LED_COUNT 22
#define DELAY 15
#define OFFSET 64 // Used to distribute LED positions in gamma array

#define MODE_BUTTON 2

#define SHUTDOWN 0
#define STEADY 1
#define FADE 2

#define Y_R 255
#define Y_G 200
#define Y_B 200

#define STEADY_BRIGHTNESS_PERC 0.5

int mode = FADE;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

const uint16_t PROGMEM gamma16[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255,
  255,252,249,247,244,241,239,236,233,231,228,225,223,220,218,215,
  213,210,208,205,203,200,198,196,193,191,189,186,184,182,180,177,
  175,173,171,169,167,164,162,160,158,156,154,152,150,148,146,144,
  142,140,138,137,135,133,131,129,127,126,124,122,120,119,117,115,
  114,112,110,109,107,105,104,102,101, 99, 98, 96, 95, 93, 92, 90,
   89, 87, 86, 85, 83, 82, 81, 79, 78, 77, 75, 74, 73, 72, 70, 69,
   68, 67, 66, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 52, 51,
   50, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 39, 38, 37,
   36, 35, 35, 34, 33, 32, 32, 31, 30, 29, 29, 28, 27, 27, 26, 25, 
   25, 24, 24, 23, 22, 22, 21, 21, 20, 20, 19, 19, 18, 18, 17, 17,
   16, 16, 15, 15, 14, 14, 13, 13, 13, 12, 12, 11, 11, 11, 10, 10,
   10,  9,  9,  9,  8,  8,  8,  7,  7,  7,  7,  6,  6,  6,  6,  5,
    5,  5,  5,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  2,
    2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};


void setup() {
  strip.begin();
  strip.clear();
  strip.show();
  pinMode(MODE_BUTTON, INPUT_PULLUP);
}

void loop() {
  if(mode == SHUTDOWN) {
    strip.clear();
    strip.show();
  } 
  else if(mode == STEADY) {
    steady();
  } else if(mode == FADE) {
    cross_fade();
  }
  check_mode_change();
  delay(10);
}

void check_mode_change() {
  if(digitalRead(MODE_BUTTON) == LOW) {
    wait_for_button_up();
    change_mode();
  }
}

void wait_for_button_up() {
  while (digitalRead(MODE_BUTTON) == LOW) {
    delay(10);
  }
}


void change_mode() {
  if(mode < 2) {
    mode++;
  } else {
    mode = 0;
  }
}

void steady() {
  for(int i=0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, (int)Y_R*STEADY_BRIGHTNESS_PERC, (int)Y_G*STEADY_BRIGHTNESS_PERC, (int)Y_B*STEADY_BRIGHTNESS_PERC);
  }
  strip.show();
}

void cross_fade() {
  for(int i=0; i < 512; i++) {
    for(int j=0; j<LED_COUNT; j++) {
      uint8_t result = pgm_read_byte(&gamma16[(i+OFFSET*j)%512]);
      float intensity_pct = result / 255.0;
      strip.setPixelColor(j, (int)Y_R*intensity_pct, (int)Y_G*intensity_pct, (int)Y_B*intensity_pct);
      check_mode_change();
      if(mode != FADE) {
        return;
      }
    }
    strip.show();
    delay(DELAY);
  }
}