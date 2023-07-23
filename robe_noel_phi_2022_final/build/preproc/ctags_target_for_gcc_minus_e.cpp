# 1 "C:\\Users\\guill\\OneDrive\\Documents\\Arduino\\arduino-etextile\\robe_noel_phi_2022_final\\robe_noel_phi_2022_final.ino"
# 2 "C:\\Users\\guill\\OneDrive\\Documents\\Arduino\\arduino-etextile\\robe_noel_phi_2022_final\\robe_noel_phi_2022_final.ino" 2
# 3 "C:\\Users\\guill\\OneDrive\\Documents\\Arduino\\arduino-etextile\\robe_noel_phi_2022_final\\robe_noel_phi_2022_final.ino" 2
# 4 "C:\\Users\\guill\\OneDrive\\Documents\\Arduino\\arduino-etextile\\robe_noel_phi_2022_final\\robe_noel_phi_2022_final.ino" 2
# 12 "C:\\Users\\guill\\OneDrive\\Documents\\Arduino\\arduino-etextile\\robe_noel_phi_2022_final\\robe_noel_phi_2022_final.ino"
unsigned long last_millis = 0;

byte gammatable[256];

// Light mode, 0 = off, 1 = beat (reset to white), 2 = steady 
int mode = 1;

Adafruit_TCS34725 tcs = Adafruit_TCS34725((0x00) /**< 614.4ms - 256 cycles - Max Count: 65535 */, TCS34725_GAIN_1X);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, 6, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);

uint32_t strip_color = strip.Color(255,255,255);


void setup() {
  // Setup Serial connection
  Serial.begin(9600);

  // Setup pin mode
  pinMode(2, 0x0);
  pinMode(8, 0x2);
  pinMode(9, 0x2);
  pinMode(6, 0x1);

  // Initialize strip
  strip.begin();
  strip.setBrightness(255);
  strip.clear();
  strip.show();

  // Detect TCS
  if(tcs.begin()) {
    Serial.println("Found sensor");
    tcs.setInterrupt(true);
  } else {
    Serial.println("Sensor not found");
    // while(1);
  }

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
  if(mode == 0) {
    light_down();
  } else if(digitalRead(2) == 0x1 && mode == 1) {
    light_up();
  } else if (mode == 1 ) {
    light_down();
  } else if (mode == 2) {
    light_up();
  }

  if(millis() - last_millis > 1000) {
    if(digitalRead(8) == 0x0) {
      change_color();
    }
    if(digitalRead(9) == 0x0) {
      change_mode();
    }
    last_millis = millis();
  }
}

void light_up() {
  Serial.println("UP");
  for (int i = 0; i < 4; i++) {
    strip.setPixelColor(i, strip_color);
  }
  strip.show();

}

void light_down() {
  Serial.println("DOWN");
  for (int i = 0; i < 4; i++) {
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
    for (int j = 0; j < 4; j++) {
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
  for(int i = 0; i < 4; i++) {
    strip.setPixelColor(i, colors[i]);
    strip.show();
    delay(500);
  }
}
