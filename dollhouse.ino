#include <Adafruit_NeoPixel.h>

#define LED_PIN 8
#define ROOF_PIN_1 9
#define ROOF_PIN_2 10

#define BUTTON_EG 3
#define BUTTON_OG 4
#define BUTTON_DG 5
#define BUTTON_ROOF 6

#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

// here is where we define the buttons that we'll use. button "1" is the first, button "6" is the 6th, etc
byte buttons[] = {BUTTON_EG, BUTTON_OG, BUTTON_DG, BUTTON_ROOF};
// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
// we will track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, LED_PIN, NEO_GRB + NEO_KHZ800);

int neopixels[] = {0, 1, 2, 3, 4, 5, 6};
int ledsDG[] = {0};
int ledsEG[] = {1, 2, 3};
int ledsOG[] = {4, 5, 6};
                  
int lightStatusEG = 0;
int lightStatusOG = 0;
int lightStatusDG = 0;
int lightStatusRoof = LOW;

void setup() {
  Serial.begin(115200);
  byte i;
    
  for (int p = 0; p < NUMBUTTONS; p++) {
    int buttonPin = buttons[p];
    pinMode(buttonPin, INPUT_PULLUP);
  }

  strip.setBrightness(200);
  strip.begin();
  setToBlack();
}

void loop() {
  //shine();
    
  checkSwitches();
  setLightStates();
}

int getStateForButton(int buttonPin) {
  switch (buttonPin) {
    case BUTTON_EG: 
      return lightStatusEG;
    case BUTTON_OG: 
      return lightStatusOG;
    case BUTTON_DG: 
      return lightStatusDG;
    case BUTTON_ROOF: 
      return lightStatusRoof;
  }  
}

void checkSwitches() {
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  //if (millis() // we wrapped around, lets just try again
  //   lasttime = millis();
  //}
  
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  
  for (index = 0; index < NUMBUTTONS; index++) { // when we start, we clear out the "just" indicators
    justreleased[index] = 0;
     
    currentstate[index] = digitalRead(buttons[index]);   // read the button
    
    //Serial.print(index, DEC);
    //Serial.print(": cstate=");
    //Serial.print(currentstate[index], DEC);
    //Serial.print(", pstate=");
    //Serial.print(previousstate[index], DEC);
    //Serial.print(", press=");
    
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
          // just pressed
          justpressed[index] = 1;
      } else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
          // just released
          justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    
    //Serial.println(pressed[index], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }

  //Serial.println("");
}

void setLightStates() {
  for (byte i = 0; i < NUMBUTTONS; i++) {
    int buttonPin = buttons[i];
    
    if (justpressed[i]) {
      //Serial.print(i, DEC);
      //Serial.println(" Just pressed"); 
      // remember, check_switches() will CLEAR the 'just pressed' flag
    }
    
    if (justreleased[i]) {
      //Serial.print(i, DEC);
      //Serial.println(" Just released");
      // remember, check_switches() will CLEAR the 'just pressed' flag
      updateLightState(buttonPin);
      updateRooms();
      updateRoof();
    }
    
    if (pressed[i]) {
      //Serial.print(i, DEC);
      //Serial.println(" pressed");
      // is the button pressed down at this moment
    }
  }  
}

void updateLightState(int buttonPin) {
  Serial.print("Update light state for button pin: ");
  Serial.println(buttonPin);
  
  switch (buttonPin) {
    case BUTTON_EG: 
      Serial.print("EG before: ");
      Serial.println(lightStatusEG);
      lightStatusEG = toggleRoomLightState(lightStatusEG);
      Serial.print("EG after: ");
      Serial.println(lightStatusEG);
      break;
    case BUTTON_OG: 
      lightStatusOG = toggleRoomLightState(lightStatusOG);
      break;
    case BUTTON_DG: 
      lightStatusDG = toggleRoomLightState(lightStatusDG);
      break;
    case BUTTON_ROOF: 
      lightStatusRoof = toggleRoofLightState(lightStatusRoof);
      break;
  }
}

int toggleRoomLightState(int currentLightState) {
  if (currentLightState == 0) {
    return 255;  
  } else {
    return 0;
  }  
}

int toggleRoofLightState(bool currentLightState) {
  return (currentLightState == HIGH) ? LOW : HIGH;
}

void updateRooms() {
  strip.setPixelColor(0, strip.Color(lightStatusDG, lightStatusDG, lightStatusDG));  
  strip.setPixelColor(1, strip.Color(lightStatusOG, lightStatusOG, lightStatusOG)); 
  strip.setPixelColor(2, strip.Color(lightStatusOG, lightStatusOG, lightStatusOG)); 
  strip.setPixelColor(3, strip.Color(lightStatusOG, lightStatusOG, lightStatusOG)); 
  strip.setPixelColor(4, strip.Color(lightStatusEG, lightStatusEG, lightStatusEG));
  strip.setPixelColor(5, strip.Color(lightStatusEG, lightStatusEG, lightStatusEG)); 
  strip.setPixelColor(6, strip.Color(lightStatusEG, lightStatusEG, lightStatusEG));
  
  //Serial.println("updateRooms()");
  //delay(3000);
  strip.show();
}

void updateRoof() {
  /**
  if (lightStatusRoof == 1) {
    rainbowCycle(20);
    lightStatusRoof = 0;
    updateRooms();
  }
  */

  Serial.println(lightStatusRoof);
  digitalWrite(ROOF_PIN_1, lightStatusRoof);
  digitalWrite(ROOF_PIN_2, lightStatusRoof);
}

void examples() {
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

void setToBlack() {
  for (int l; l < sizeof(neopixels); l++) {
    strip.setPixelColor(neopixels[l], strip.Color(0, 0, 0));
  } 
          
  strip.show();

  digitalWrite(ROOF_PIN_1, LOW);
  digitalWrite(ROOF_PIN_2, LOW);
}

void shine() {
  strip.setPixelColor(0, strip.Color(255, 0, 0)); //DG
  strip.setPixelColor(1, strip.Color(0, 255, 0)); //OG rechts
  strip.setPixelColor(2, strip.Color(0, 255, 0)); //OG rechts
  strip.setPixelColor(3, strip.Color(0, 0, 255)); //OG links
  strip.setPixelColor(4, strip.Color(255, 255, 0)); //EG links
  strip.setPixelColor(5, strip.Color(127, 127, 127)); //EG rechts
  strip.setPixelColor(6, strip.Color(127, 127, 127)); //EG rechts
        
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


