#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <FastLED.h>

/* ================= TFT (ESP32-S3) ================= */
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/* ================= FASTLED ================= */
#define LED_PIN     42
#define NUM_LEDS    40
#define BRIGHTNESS  100
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define PIN_40 40
#define PIN_41 41

CRGB leds[NUM_LEDS];

/* ================= TIMERS ================= */
unsigned long t200, t376, t1s, tBlinkFast;

/* ================= ANIMATION STATE ================= */
int run_2_5     = 1;
int run_6_9     = 5;
int dir_6_9     = 1;
int run_10_13   = 9;
int dir_10_13   = 1;
int run_19_22   = 18;

bool blink16 = false;

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  /* ----- TFT INIT ----- */
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);               // LANDSCAPE, SEEDHA
  tft.fillScreen(ST77XX_BLACK);

  tricorderBootAnimation();
  delay(800);
  showTricorderScreen();

  /* ----- FASTLED INIT ----- */
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(PIN_40, INPUT);
  pinMode(PIN_41, INPUT);
}

/* ================= LOOP ================= */
void loop() {
  runLedAnimations();
}

/* ================================================= */
/* ================= TFT FUNCTIONS ================= */
/* ================================================= */

void tricorderBootAnimation() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setTextWrap(false);

  const char *title = "STAR TREK'S";
  const char *subtitle = "TRICORDER";

  int16_t x1, y1;
  uint16_t w, h;

  tft.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 30);
  for (int i = 0; i < strlen(title); i++) {
    tft.print(title[i]);
    delay(80);
  }

  delay(300);

  tft.getTextBounds(subtitle, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 65);
  for (int i = 0; i < strlen(subtitle); i++) {
    tft.print(subtitle[i]);
    delay(80);
  }

  for (int y = 0; y < tft.height(); y += 4) {
    tft.drawFastHLine(0, y, tft.width(), ST77XX_GREEN);
    delay(10);
  }
}

void showTricorderScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextWrap(false);

  tft.setTextColor(ST77XX_CYAN);
  tft.println("TRICORDER-108   v1.3.2");

  tft.setTextColor(ST77XX_WHITE);
  tft.println("--------------------------------");
  tft.println("ENVIRONMENT SCAN\n");

  tft.setTextColor(ST77XX_GREEN);
  tft.print("Temperature : ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("28.6 C");

  tft.setTextColor(ST77XX_GREEN);
  tft.print("Humidity    : ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("61 %");

  tft.setTextColor(ST77XX_GREEN);
  tft.print("Pressure    : ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("101.3 kPa\n");

  tft.setTextColor(ST77XX_YELLOW);
  tft.println("Air Quality : NORMAL");

  tft.setTextColor(ST77XX_GREEN);
  tft.println("Radiation   : SAFE\n");

  tft.setTextColor(ST77XX_WHITE);
  tft.println("Battery     : 82 %");
  tft.print("Signal      : ");
  tft.print((char)219);
  tft.print((char)219);
  tft.print((char)219);
  tft.print((char)219);
  tft.print((char)176);
}

/* ================================================= */
/* ================= FASTLED LOOP ================= */
/* ================================================= */

void runLedAnimations() {
  unsigned long now = millis();
  FastLED.clear();

  /* ----- Static LEDs ----- */
  leds[0]  = CRGB::Orange;
  leds[13] = CRGB::Red;
  leds[14] = CRGB::Red;
  leds[17] = CRGB::Blue;
  leds[22] = CRGB::Yellow;
  leds[23] = CRGB::Green;
  leds[24] = CRGB::Yellow;
  leds[25] = CRGB::Yellow;

  /* ----- Running Green (2–5) ----- */
  if (now - t200 > 200) {
    t200 = now;
    run_2_5++;
    if (run_2_5 > 4) run_2_5 = 1;
  }
  leds[run_2_5] = CRGB::Green;

  /* ----- Cyan Back & Forth (6–9) ----- */
  if (now - tBlinkFast > 150) {
    tBlinkFast = now;
    run_6_9 += dir_6_9;
    if (run_6_9 == 8 || run_6_9 == 5) dir_6_9 *= -1;
  }
  leds[run_6_9] = CRGB::Cyan;

  /* ----- Cyan Back & Forth (10–13) ----- */
  if (now - tBlinkFast > 150) {
    run_10_13 += dir_10_13;
    if (run_10_13 == 12 || run_10_13 == 9) dir_10_13 *= -1;
  }
  leds[run_10_13] = CRGB::Cyan;

  /* ----- Orange Blink (16–17) ----- */
  if (now - t1s > 1000) {
    t1s = now;
    blink16 = !blink16;
  }
  if (blink16) {
    leds[15] = CRGB::Orange;
    leds[16] = CRGB::Orange;
  }

  /* ----- Running Green (19–22) ----- */
  if (now - t376 > 376) {
    t376 = now;
    run_19_22++;
    if (run_19_22 > 21) run_19_22 = 18;
  }
  leds[run_19_22] = CRGB::Green;

  /* ----- Yellow Blink (27) ----- */
  if (blink16) leds[26] = CRGB::Yellow;

  /* ----- Red Blink (28–30) ----- */
  if (blink16) {
    leds[27] = CRGB::Red;
    leds[28] = CRGB::Red;
    leds[29] = CRGB::Red;
  }

  /* ----- GPIO Trigger ----- */
  if (digitalRead(PIN_40) == HIGH && digitalRead(PIN_41) == HIGH) {
    leds[30] = CRGB::Purple;
  }

  FastLED.show();
}
