#include <Adafruit_NeoPixel.h>
#include <PinChangeInt.h>

// For PinChangeInt - No interrupts on ports C or D
//#define NO_PORTC_PINCHANGES
//#define NO_PORTD_PINCHANGES

#define LED_PIN 12      //D12

#define BUTTON_0 6      //D6
#define BUTTON_1 9      //D9
#define BUTTON_2 10     //D10
#define BUTTON_DOOR 1  //TX

#define SB_DATA 2      //SDA
#define SB_CLK 3       //SCL
#define SB_BUSY 0       //RX

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, LED_PIN, NEO_GRB + NEO_KHZ800);

void(*modes[]) () = { };
                  
buttonPins = [6, 9, 10];

ledsDG = [0];
ledsEG = [1,2,3];
ledsOG = [4,5,6];
                  
lightStatusEG = 0;
lightStatusOG = 0;
lightStatusDG = 0;
statusGong = 0;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  foreach (buttonPins as buttonPin) {
    pinMode(buttonPin, INPUT_PULLUP);
    PCintPort::attachInterrupt(buttonPin, &pressHandler, FALLING);
  }
  
  strip.setBrightness(90);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  shine();
  
//  https://learn.adafruit.com/florabrella/code
  
  static int mode = 0;  
  if (changeMode != 0) {
    //Serial.println("change");
    mode = ((mode + 1) % numModes);
    resetToBlack();
    changeMode = 0;
  }
  (*modes[mode])(); // call the new mode function
  
  //examples();
}

void checkButtonState(buttonPin, currentState) {
  val = digitalRead(buttonPin); // read input value and store it in val
  delay (20);
  val2 = digitalRead(buttonPin);
  if (val == val2) {
    if (val != currentState && val == LOW) { // the button state has changed!
        if (lightMode == 0) {
          lightMode = 1;
        }
      else if (lightMode == 1) {
          lightMode = 2;
        }
      else if (lightMode == 2){
          lightMode = 3;
          //delay (20);
        }
      else if (lightMode == 3) {
          lightMode = 0;
        }
      }
    }

  currentState = val; // save the new state in our variable
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

void pressHandler() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  // debounce
  if (interruptTime - lastInterruptTime > 200) {
    changeMode = 1;
    lastInterruptTime = interruptTime;
  }
}

