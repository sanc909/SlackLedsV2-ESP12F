/**
   Arduino Real-Time Slack Bot

   Copyright (C) 2016, Uri Shaked.

   Licensed under the MIT License
*/
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define LEDS_PIN        2
#define LEDS_NUMPIXELS  7
#define WORD_SEPERATORS "., \"'()[]<>;:-+&?!\n\t"


Adafruit_NeoPixel pixels(LEDS_NUMPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800);

uint32_t wheel(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return pixels.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return pixels.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return pixels.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

/**
  Animate a NeoPixel ring color change.
  Setting `zebra` to true skips every other led.
*/
void drawColor(uint32_t color, bool zebra) {
  int step = zebra ? 2 : 1;
  for (int i = 0; i < LEDS_NUMPIXELS; i += step) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(30 * step);
  }
}

/**
  Draws a rainbow :-)
*/
void drawRainbow(bool zebra) {
  int step = zebra ? 2 : 1;
  for (int i = 0; i < LEDS_NUMPIXELS; i += step) {
    pixels.setPixelColor(i, wheel(i * 256 / LEDS_NUMPIXELS));
    pixels.show();
    delay(30 * step);
  }
}


/**
  Looks for color names in the incoming slack messages and
  animates the ring accordingly. You can include several
  colors in a single message, e.g. `red blue zebra black yellow rainbow`
*/
void processSlackMessage(char *payload) {
  char *nextWord = NULL;
  bool zebra = false;
  float brightness; 
  for (nextWord = strtok(payload, WORD_SEPERATORS); nextWord; nextWord = strtok(NULL, WORD_SEPERATORS)) {
  
    if (strcasecmp(nextWord, "zebra") == 0) {
      zebra = true;
    }
    if (strncmp(nextWord, "brightness",9) == 0) {                //Syntax is brightness100 etc no space.
      
      brightness = (float) strtol(&nextWord[10], NULL, 10)/100;
           
    }
    
    if (strcasecmp(nextWord, "red") == 0) {
      drawColor(pixels.Color(255*brightness, 0, 0), zebra);
    }
    if (strcasecmp(nextWord, "green") == 0) {
      drawColor(pixels.Color(0, 255*brightness, 0), zebra);
    }
    if (strcasecmp(nextWord, "blue") == 0) {
      drawColor(pixels.Color(0, 0, 255*brightness), zebra);
    }
    if (strcasecmp(nextWord, "yellow") == 0) {
      drawColor(pixels.Color(255*brightness, 160*brightness, 0), zebra);
    }
    if (strcasecmp(nextWord, "white") == 0) {
      drawColor(pixels.Color(255*brightness, 255*brightness, 255*brightness), zebra);
    }
    if (strcasecmp(nextWord, "purple") == 0) {
      drawColor(pixels.Color(128*brightness, 0, 128*brightness), zebra);
    }
    if (strcasecmp(nextWord, "pink") == 0) {
      drawColor(pixels.Color(255*brightness, 0, 96*brightness), zebra);
    }
    if (strcasecmp(nextWord, "orange") == 0) {
      drawColor(pixels.Color(255*brightness, 64*brightness, 0), zebra);
    }
    if (strcasecmp(nextWord, "black") == 0) {
      drawColor(pixels.Color(0, 0, 0), zebra);
    }
    if (strcasecmp(nextWord, "rainbow") == 0) {
      drawRainbow(zebra);
    }
    if (nextWord[0] == '#') {
      int color = strtol(&nextWord[1], NULL, 16);
      Serial.println("Color");
      Serial.print(color);
      if (color) {
        drawColor(color, zebra);
      }
    }
  }
}

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  pixels.begin();
  processSlackMessage("black "); 
  
}

// every 0.5 secs read strings sent via serial port 
void loop() {

  //byte serin=0; 
  String serin; 
  
  if (Serial.available()>0)
  {
    serin=Serial.readString();
    delay(100);
    Serial.print(serin);
    
    char buf[serin.length()];
    serin.toCharArray(buf,serin.length());
    processSlackMessage(&buf[0]);
    
  }
  else
  {
    delay(500);
  }

}

