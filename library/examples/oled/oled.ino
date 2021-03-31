#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "Arduino.h"
#include "ramen.h"

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555

ramen consensus_on_mesh_server;

unsigned long previousMillis = 0;
const long interval = 1000;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Initialize ramen
  consensus_on_mesh_server.init(MESH_NAME,
                                MESH_PASSWORD,
                                MESH_PORT,
                                broth::logger::DEBUG);
}

void loop() {
  consensus_on_mesh_server.update();

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    display.clearDisplay();
    display.setCursor(0, 10);

    switch(consensus_on_mesh_server.getState()) {
      case 0:
        display.println("FOLLOWER");
        break;
      case 1:
        display.println("CANDIDATE");
        break;
      case 2:
        display.println("LEADER");
        break;
      default:
        break;
    }

    display.display();
  }
}
