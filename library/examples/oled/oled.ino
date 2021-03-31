#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "Arduino.h"
#include "PCA9635.h"
#include "ramen.h"

// OLED CONFIG
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PWM DRIVER CONFIG
#define PCA9635_ADDR 0x15
PCA9635 ledArray(PCA9635_ADDR);

// RAMEN CONFIG
#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555
ramen ramen_server;

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;)
      ;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Initialize ramen
  ramen_server.init(MESH_NAME, MESH_PASSWORD, MESH_PORT, broth::logger::DEBUG);

  bool connected_one = ledArray.begin(4, 5);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // IMPORTANT!
  // Inversion necessary
  ledArray.writeMode(PCA9635_MODE2, 21);

  for(int channel = 0; channel < 16; channel++) {
    ledArray.setLedDriverMode(channel, PCA9635_LEDOFF);
  }

  // Turn on PWM mode for RGB on LED 1
  for(int channel = 0; channel < 3; channel++) {
    ledArray.setLedDriverMode(channel, PCA9635_LEDPWM);
    ledArray.write1(channel, 0);
  }
}

void loop() {
  ramen_server.update();

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    display.clearDisplay();
    display.setCursor(0, 10);

    switch(ramen_server.getState()) {
      case 0:
        display.println("FOLLOWER");
        ledArray.write3(0, 5, 0, 0); // R
        break;
      case 1:
        display.println("CANDIDATE");
        ledArray.write3(0, 0, 0, 5); // B
        break;
      case 2:
        display.println("LEADER");
        ledArray.write3(0, 0, 5, 0); // G

        break;
      default:
        break;
    }

    display.display();
  }
}

//
//    FILE: PCA9635_test01.ino
//  AUTHOR: Rob Tillaart
//    DATE: 23-APR-2016
// VERSION: 0.2.0
// PUPROSE: test PCA9635 library
//

// #include <Wire.h>

// #include "PCA9635.h"

// PCA9635 ledArray(0x15);

// void setup() {
//   Serial.begin(115200);
//   Serial.print("PCA9635 LIB version: ");
//   Serial.println(PCA9635_LIB_VERSION);
//   Serial.println();

//   pinMode(13, OUTPUT);
//   digitalWrite(13, HIGH);

//   if(!ledArray.begin()) {
//     Serial.println("Connection Failed!");
//     return;
//   }

//   testPWMMode();
//   testWrite1();

//   Serial.print(millis());
//   Serial.print("\t");
//   Serial.println("done...");
// }

// void testPWMMode() {
//   Serial.print(millis());
//   Serial.print("\t");
//   Serial.println("Test - set pwm mode");
//   for(int channel = 0; channel < 16; channel++) {
//     ledArray.setLedDriverMode(channel, PCA9635_LEDON);
//     delay(200);
//     ledArray.setLedDriverMode(channel, PCA9635_LEDPWM);
//     if(ledArray.getLedDriverMode(channel) != PCA9635_LEDPWM) {
//       Serial.print(millis());
//       Serial.print("\t");
//       Serial.print("Channel: ");
//       Serial.println(channel);
//     }
//   }
// }

// void testWrite1() {
//   Serial.print(millis());
//   Serial.print("\t");
//   Serial.println("Test - write1 - I");
//   for(int channel = 0; channel < 16; channel++) {
//     for(int pwm = 255; pwm > 0; pwm--) {
//       ledArray.write1(channel, pwm);
//     }
//     delay(50);
//     ledArray.setLedDriverMode(channel, PCA9635_LEDON);
//   }
// }

// void loop() {
// }