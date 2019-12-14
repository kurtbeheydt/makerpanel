#include <Adafruit_NeoPixel.h>
#include <AceButton.h>            // https://github.com/bxparks/AceButton
using namespace ace_button;

#define BUTTON_PIN 5
AceButton button(BUTTON_PIN);

#define LED_PIN    4
#define LED_COUNT 288
#define LED_DEFAULT_INTERVAL 5

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t stripColor[144];

int i;

bool flameAction = false;

long flamePcnt = 0;
const long flameTimeOut = 10000;
long flameTimePassed = 0;


void generateLedLine() {
  int red = random(64, 255);
  int yellow = random(0, 100);
  if (yellow > 90) {
    yellow = random(150, 255);
    red = random(150, 255);
  } else {
    yellow = 0;
  }
  
  uint32_t color = strip.Color(red, yellow, 0);
  stripColor[0] = color;
}

void shiftNext() {
  for (i = 144; i > 0; i--) {
    stripColor[i] = stripColor[i-1];
  }
}

void showStrip() {
  strip.clear();
  for (i = 0; i < 144; i++) {
    strip.setPixelColor(i, stripColor[i]);
    strip.setPixelColor(i+144, stripColor[i]);
  }
  strip.show();
}


void enableFlameAction() {
  flameAction = true;
  flameTimePassed = millis();
}

void handleEvent(AceButton* , uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      enableFlameAction();
      break;
  }
}

void setup() {
  randomSeed(analogRead(0));  

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

  strip.begin();
  strip.setBrightness(250);
  strip.show();
  showStrip();
  generateLedLine();

}

void loop() {
  button.check();
  
  if (millis() - flameTimePassed >= flameTimeOut) {
    flameAction = false;
  }
  digitalWrite(LED_BUILTIN, flameAction);

  long flameDelay = (flameAction) ? 10000 : 50000;
  
  if (flamePcnt >= flameDelay) {
    flamePcnt = 0;
    shiftNext();
    generateLedLine();
    showStrip();
  }

  flamePcnt += LED_DEFAULT_INTERVAL;
}
