#include "Arduino.h"
#include "ramen.h"

#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555

ramen consensus_on_mesh_server;

unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize ramen
  consensus_on_mesh_server.init(MESH_NAME,
                                MESH_PASSWORD,
                                MESH_PORT,
                                broth::logger::DEBUG);

  // Set the builtin blue LED as output
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {
  consensus_on_mesh_server.update();

  // If the server is a leader, turn on the blue LED
  if(consensus_on_mesh_server.getState() == broth::server::LEADER) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Show remaining memory
    Serial.print("Remaining memory: ");
    Serial.println(ESP.getFreeHeap());
  }
}