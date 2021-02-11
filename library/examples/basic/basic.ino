#include "Arduino.h"
#include "ramen.h"

#define MESH_NAME     "ramen"
#define MESH_PASSWORD "ramen123*"
#define MESH_PORT     5555

ramen consensus_on_mesh_server;

void setup() {
  Serial.begin(115200);
  consensus_on_mesh_server.init(MESH_NAME, MESH_PASSWORD, MESH_PORT);
}

void loop() {
  consensus_on_mesh_server.update();
}