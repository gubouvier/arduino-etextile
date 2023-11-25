// TODO: Regler le probleme de 'flickering' en base intensite.  (variation de la lecture analogue du potentio)


#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TCS34725.h>

#define DEBUG_ALL 9
#define DEBUG_MODE_CHANGE 5
#define DEBUG_OFF 0

#define DEBUG DEBUG_OFF

#define DELAY 100

#define LED_STRIP_DATA 6
#define PIN_BRIGHTNESS A0
#define PIN_MODE_CHANGE 7
#define PIN_CHECK_COLOR 8

#define NUMPIXELS 100
#define INTENSITE_MAX 150
#define INTENSITE_MIN 1

#define POT_MIN_VALUE 512

#define MODE_OFF -1
#define MODE_STEADY 0
#define MODE_FADE 1

#define FADE_DELAY 20000

Adafruit_NeoPixel strip(NUMPIXELS, LED_STRIP_DATA, NEO_RGB + NEO_KHZ800);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

byte gammatable[256];

int mode = MODE_STEADY;

int intensite_table[10]; 
int intensite = INTENSITE_MAX;
int intensite_pointer = 0;

unsigned long last_millis = 0;
unsigned long color_timer = 0;

boolean color_changed = false;

uint32_t strip_color;


void setup() {
  Serial.begin(9600);
  pinMode(LED_STRIP_DATA, OUTPUT);
  pinMode(PIN_BRIGHTNESS, INPUT);
  pinMode(PIN_MODE_CHANGE, INPUT_PULLUP);
  pinMode(PIN_CHECK_COLOR, INPUT_PULLUP);

  strip.begin();
  strip.clear();
  strip.show();

  while(!tcs.begin()){
    Serial.println("TCS not ready, sleeping 100");
    delay(100);
  }
  Serial.println("TCS Ready, setting interrupt to False");
  tcs.setInterrupt(true);
  Serial.println("TCS interrupt to false");  

  init_gammatable();
  init_intensite_table();
  strip_color = strip.Color(gammatable[intensite], gammatable[intensite], gammatable[intensite]);
}

void loop() {
  intensite = get_intensite();
  Serial.println(intensite);

  if(digitalRead(PIN_MODE_CHANGE) == LOW) {
    wait_pin_up(PIN_MODE_CHANGE);
    change_mode();
  }
  if(digitalRead(PIN_CHECK_COLOR) == LOW) {
    wait_pin_up(PIN_CHECK_COLOR);
    strip_color = get_color_tcs();
    color_timer = millis();
    color_changed = true;
  }

  if (color_changed && (millis() - color_timer > FADE_DELAY)) {
    fade_to_white();
    color_changed = false;
  }
  
  if (mode == MODE_OFF) {
    strip.clear();
    strip.show();
  } else if (mode == MODE_STEADY) {
    set_strip_color(strip_color);
  } else if (mode == MODE_FADE) {
    fade(strip_color);
  }
  delay(DELAY);
}

void wait_pin_up(int pin) {
  while(digitalRead(pin) == LOW) {
    delay(10);
  }
}

int get_intensite() {
  int brightness_input = analogRead(PIN_BRIGHTNESS);
  if(DEBUG >= DEBUG_ALL) {
    Serial.print("brightness input: ");
    Serial.println(brightness_input);
  }
  int temp_intensite = ((float)brightness_input - POT_MIN_VALUE) / POT_MIN_VALUE * INTENSITE_MAX;
  if(DEBUG >= DEBUG_ALL) {
    Serial.print("Intensite : ");
    Serial.println(temp_intensite);
  }
  if(temp_intensite <= INTENSITE_MIN) {
    if(DEBUG >= DEBUG_ALL) {
      Serial.println("Read intensite plus petit que min, returning minimum");
    }
    intensite_table[intensite_pointer] = INTENSITE_MIN;
  } else {
    intensite_table[intensite_pointer] = temp_intensite;
  }
  if(intensite_pointer >= 9) {
    intensite_pointer = 0;
  } else {
    intensite_pointer++;
  }
  // Serial.print("Table : ");
  // for(int i=0; i<10; i++) {
  //   Serial.print(intensite_table[i]);
  //   Serial.print(", ");
  // }
  // Serial.println(" .");

  int somme = 0;
  for(int i=0; i<10; i++) {
    somme += intensite_table[i];
  }
  return somme / 10; 
}

void change_mode() {
  if(DEBUG >= DEBUG_MODE_CHANGE) {
    Serial.println("Change Mode");
  }
  if(mode < 1) {
    mode++;
  } else {
    mode = -1;
  }
  if(DEBUG >= DEBUG_MODE_CHANGE) {
    Serial.print("Mode = ");
    Serial.println(mode);
  }
}

void fade(uint32_t color) {

}

void fade_to_white() {
  // TODO: fade
  strip_color = strip.Color(gammatable[intensite], gammatable[intensite], gammatable[intensite]);
}

void set_strip_color(uint32_t color) {
  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.setBrightness(intensite);
  strip.show();
}

uint32_t get_color_tcs() {
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
  
  color = strip.Color(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
  return color;
}

void increase_brightness(float *r, float *g, float *b) {
  float max_value = find_max(*r, *g, *b);
  float multiplier = get_intensite() / max_value;
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

void init_intensite_table() {
  for(int i=0; i<10; i++) {
    intensite_table[i] = 0;
  }
  if(DEBUG >= DEBUG_ALL) {
    Serial.println("Intensite table : ");
    for(int i=0; i<10; i++) {
      Serial.print(intensite_table[i]);
      Serial.print(", ");
    }
    Serial.println(".");
  }
}

void init_gammatable() {
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = x;
  }
} 