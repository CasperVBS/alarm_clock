#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCLK 9 // labeled SCL on the screen
#define TFT_MOSI 10 // labeled SDA on the screen
#define TFT_RST 20
#define TFT_DC 6
#define TFT_CS 7
#define TFT_BL 21

#define buttonA 2
#define buttonB 3
#define buttonC 4
#define buttonD 5


void setup() {
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:

}
