#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

const char* ssid ="ssid";
const char* pswd = "pswd";

// timezone for belgium (where I live 🧇🍟🍫) 
const char* timezone = "CET-1CEST,M3.5.0,M10.5.0"; 



#define TFT_SCLK 9 // labeled SCL on the screen
#define TFT_MOSI 10 // labeled SDA on the screen
#define TFT_RST 8 
#define TFT_DC 6 
#define TFT_CS 7 
#define TFT_BL 21 

#define buttonA 2
#define buttonB 3
#define buttonC 4
#define buttonD 5
#define buzzer 20

int start_time = 0;
int passed_time = 0;
bool stopwatch = false;
bool timer_mode = false;
bool timer_set = true;

int seconds_timer = 0;
int minutes_timer = 0;

bool last_state_A = HIGH;
bool last_state_B = HIGH;
bool last_state_C = HIGH;
bool last_state_D = HIGH;

// Fix setColRowStart() by exposing it via a subclass
class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void setup() {
  Serial.begin(115200);
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);

  // setup for screen 
  tft.init(76, 284); // Our panel size (portrait)
  tft.setOffsets(82, 18); // Offsets for the weird resolution
  tft.invertDisplay(false); // Invert the colors (This display is flipped from normal)
  tft.setRotation(1); // Landscape, if it's upside down use 3!

  // start wifi 
   WiFi.begin(ssid, pswd);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.println("no wifi");
  // hopfully this will give a ... animation until connected, as of writing this I can't test it so this meight need to be changed
    tft.print(".");
  }
  
  tft.println("conected to wifi");
  delay(20);
  tft.fillScreen(ST77XX_BLACK); // clear the screen
  configTzTime(timezone, "pool.ntp.org", "time.nist.gov"); // -> loads timezone data
}

void loop(){
  bool buttonStateA = digitalRead(buttonA);
  bool buttonStateB = digitalRead(buttonB);
  bool buttonStateC = digitalRead(buttonC);
  bool buttonStateD = digitalRead(buttonD);

  if (last_state_A == HIGH && buttonStateA == LOW){
    //start selecting mode 
    timer_mode = true;
    seconds_timer = 0;
    minutes_timer = 0;
  }

  if (timer_mode && !timer_set){
    bool minutes_set = false;
    bool seconds_set = false;

    // time up button
    if(last_state_B == HIGH && buttonStateB == LOW){
      if(!seconds_set && seconds_timer < 60){
        seconds_timer += 1;
      }
      else{
        minutes_timer += 1;
      }
    }
    // time down button 
    if(last_state_C == HIGH && buttonStateC == LOW){
      if(!seconds_set && seconds_timer < 0){
        seconds_timer -= 1;
      }
      else{
        minutes_timer -= 1;
      }
    }
    
    // start button
    if(last_state_D == HIGH && buttonStateD == LOW){
      if (!seconds_set){
        seconds_set = true;
      } else if (!minutes_set){
        seconds_set = false;
        timer_set = true;
      }
    }
  if (timer_set){
    unsigned long timer_start = millis();
    unsigned long milis_set_time = (minutes_timer*60 + seconds_timer)*1000;
    unsigned long time_passed = millis() - timer_start;
    if (time_passed < milis_set_time){
      unsigned long time_to_go = milis_set_time - time_passed;
      // convurting it for on the screen 
      unsigned long time_to_go_sec = time_to_go / 1000;
      unsigned long minutes_show = time_to_go_sec / 60;
      unsigned long seconds_show = time_to_go_sec %60; 

      tft.fillScreen(ST77XX_BLACK);
      if (minutes_show < 10){
        tft.print("0");
      }
      tft.print(minutes_show);
      if (seconds_show < 10){
        tft.print("0");
      }
      tft.print(seconds_show);
      }
    else{
      for (int i = 0; i < 5; i++){
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, 200);
        delay(200);
        digitalWrite(buzzer, 500);
        delay(200);
        
      }
    }
  }

  }
  // if timer mode display time
  if (timer_mode && !timer_set){
    tft.fillScreen(ST77XX_BLACK);
    if (minutes_timer < 10){
      tft.print("0");
    }
    tft.print(minutes_timer);
    if (seconds_timer < 10){
      tft.print("0");
    }
    tft.print(seconds_timer);
  }

  last_state_A = buttonStateA;
  last_state_B = buttonStateB;
  last_state_C = buttonStateC;
  last_state_D = buttonStateD;

   if (!timer_mode && !stopwatch) {
    clock_mode();
  }
}

void clock_mode(){
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return;
  }
  char timeString[6];
  strftime(timeString, sizeof(timeString), "%H:%M", &timeinfo);

}

void stopwatch_mode() {
  bool buttonStateA = digitalRead(buttonA);
  if (last_state_A == HIGH && buttonStateA == LOW){
    if (!stopwatch){
      start_time = millis() - passed_time;
      stopwatch = true;
    }else {
      passed_time = millis()- start_time;
      stopwatch = false;
    }
  }
  if (stopwatch){
    passed_time = millis()- start_time;

    unsigned long time_seconds = passed_time / 1000;
    unsigned long minutes = time_seconds / 60;
    unsigned long seconds = time_seconds %60; 

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(50,50);
    tft.setTextSize(5);
    if (minutes < 10){
      tft.print("0");
    }
    tft.print(minutes);
    if (seconds < 10){
      tft.print("0");
    }
    tft.print(seconds);


  }

}




/*tft.print("STRING HERE");  // prints out the string
tft.setCursor(x,y);        // the screen is 284x76! Think of it as a x,y grid
tft.fillScreen(ST77XX_COLOR); // most of the main colors are here!
tft.drawRect(Cursorx, Cursory, RectWidth, RectHeight, ST77XX_color); // draws a rectangle
tft.fillRect(Cursorx, Cursory, RectWidth, RectHeight, ST77XX_color); // fills in a rectangle
tft.setTextSize(integer);  // try it out! Use 1,2,3, etc.*/