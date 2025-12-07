#include <LiquidCrystal.h>

// LCD pins fixed by the shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// LED pins on your shield header
const int RED_LED   = 3;   // Shadow
const int GREEN_LED = 11;  // Light

// Button values from your hardware
int adc_key_val[5] = {0, 221, 408, 625, 824};
#define KEY_RIGHT  0
#define KEY_UP     1
#define KEY_DOWN   2
#define KEY_LEFT   3
#define KEY_SELECT 4

// Emotion model
struct Emotion {
  const char* name;
  int weight;
  bool isDark;
};

Emotion emotions[10] = {
  {"Shame",      20,  true},
  {"Guilt",      30,  true},
  {"Apathy",     50,  true},
  {"Fear",       100, true},
  {"Desire",     125, true},
  {"Courage",    200, false},
  {"Acceptance", 350, false},
  {"Love",       500, false},
  {"Joy",        540, false},
  {"Peace",      600, false}
};

int order[10];
int intensity[10]; // 1=None, 2=Low, 3=Moderate, 4=Strong, 5=Dominant

// timing state for LEDs
unsigned long lastRedToggle = 0;
bool redOn = false;

// Read LCD buttons
int read_LCD_buttons() {
  int adc_key_in = analogRead(0);
  for (int k = 0; k < 5; k++) {
    if (adc_key_in < adc_key_val[k] + 20 && adc_key_in > adc_key_val[k] - 20)
      return k;
  }
  return -1;
}

const char* intensityLabel(int v) {
  switch (v) {
    case 1: return "None";
    case 2: return "Low";
    case 3: return "Moderate";
    case 4: return "Strong";
    case 5: return "Dominant";
  }
  return "";
}

void setup() {
  lcd.begin(16, 2);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Emotion Init...");

  // init intensities
  for (int i = 0; i < 10; i++) intensity[i] = 1;

  // random order for emotions
  for (int i = 0; i < 10; i++) order[i] = i;
  for (int i = 0; i < 10; i++) {
    int r = random(10);
    int t = order[i];
    order[i] = order[r];
    order[r] = t;
  }

  delay(1000);
  lcd.clear();
}

void loop() {
  // -------- EMOTION INPUT --------
  for (int i = 0; i < 10; i++) {
    int e = order[i];
    int val = intensity[e];

    while (true) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      lcd.setCursor(0, 0);
      lcd.print(emotions[e].name);

      lcd.setCursor(0, 1);
      lcd.print("Int: ");
      lcd.print(intensityLabel(val));

      int key = read_LCD_buttons();
      if (key == KEY_UP && val < 5)   { val++; delay(200); }
      if (key == KEY_DOWN && val > 1) { val--; delay(200); }
      if (key == KEY_SELECT) {
        intensity[e] = val;
        delay(300);
        break;
      }
    }
  }

  // ------- SCORE MODEL -------
  long darkScore = 0;
  long lightScore = 0;

  for (int i = 0; i < 10; i++) {
    int eff = (intensity[i] == 1 ? 0 : intensity[i]); // 1=None => 0 contribution
    long contrib = (long)emotions[i].weight * eff;
    if (emotions[i].isDark) darkScore += contrib;
    else                    lightScore += contrib;
  }

  long total = darkScore + lightScore;
  float shadowP = 0;
  float lightP  = 0;

  if (total > 0) {
    shadowP = (darkScore * 100.0) / total;
    if (shadowP < 0)   shadowP = 0;
    if (shadowP > 100) shadowP = 100;
    lightP = 100.0 - shadowP;
  } else {
    shadowP = 0;
    lightP = 0;
  }

  // ------- DISPLAY RESULTS -------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Shadow: ");
  lcd.print((int)shadowP);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("Light : ");
  lcd.print((int)lightP);
  lcd.print("%   ");

  delay(2000);

  // ------- LED RITUAL MODE -------
  while (true) {
    unsigned long now = millis();

    // ==== SHADOW LED (RED) — 20 levels, every 5% ====
    int s = (int)shadowP;

    if (s <= 0) {
      // 0% shadow → LED fully off
      analogWrite(RED_LED, 0);
    } else {
      // bucket 1..20 → more levels
      int bucket = s / 5;          // 0..20
      if (bucket < 1)  bucket = 1;
      if (bucket > 20) bucket = 20;

      // brightness: low for low shadow, high for high shadow
      int shadowBrightness = map(bucket, 1, 20, 15, 255); // 20 discrete steps

      // blink period: 3000ms (very slow) down to 80ms (strobe)
      int shadowPeriod = map(bucket, 1, 20, 3000, 80);

      // non-blocking blink using millis
      if (now - lastRedToggle >= (unsigned long)(shadowPeriod / 2)) {
        lastRedToggle = now;
        redOn = !redOn;
        if (redOn) analogWrite(RED_LED, shadowBrightness);
        else       analogWrite(RED_LED, 0);
      }
    }

    // ==== LIGHT LED (GREEN) — survival → expansion ====
    int l = (int)lightP;

    if (l <= 0) {
      // 0% light → LED off
      analogWrite(GREEN_LED, 0);
    } else if (l < 20) {
      // 0–20% → survival mode: mostly off, rare faint pulses
      // More light% => slightly more frequent & brighter pulses
      unsigned long period = map(l, 1, 19, 4000, 2000);   // long cycles
      unsigned long onTime = map(l, 1, 19, 50, 300);      // pulse width
      int minB = map(l, 1, 19, 10, 60);                   // faint brightness

      unsigned long phase = now % period;
      if (phase < onTime) {
        analogWrite(GREEN_LED, minB);
      } else {
        analogWrite(GREEN_LED, 0);
      }

    } else {
      // >=20% → breathing pattern (expansion)
      // More light% => faster + stronger breathing

      // 20–100% mapped to ~2000ms → 600ms breath cycle
      unsigned long breathPeriod = map(l, 20, 100, 2000, 600);
      if (breathPeriod < 600)  breathPeriod = 600;
      if (breathPeriod > 3000) breathPeriod = 3000;

      // brightness range grows with light%
      int minB = map(l, 20, 100, 20, 80);
      int maxB = map(l, 20, 100, 150, 255);
      if (minB < 0)   minB = 0;
      if (maxB > 255) maxB = 255;
      if (maxB < minB) maxB = minB;

      unsigned long phase = now % breathPeriod;
      float p = phase / (float)breathPeriod; // 0.0 - 1.0

      // simple triangle wave for breathing: up then down
      float factor;
      if (p < 0.5) {
        factor = p * 2.0;        // 0 → 1
      } else {
        factor = (1.0 - p) * 2;  // 1 → 0
      }

      int greenVal = minB + (int)(factor * (maxB - minB));
      analogWrite(GREEN_LED, greenVal);
    }

    // small pause so we don't spin too fast
    delay(20);
  }
}