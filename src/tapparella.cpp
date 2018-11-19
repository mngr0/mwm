#include <Arduino.h>
#include <painlessMesh.h>
#include "interface.h"

#define MOTOR_STEP 15 //no touch
#define MOTOR_DIR 13

#define SENS_UP 12
#define SENS_DOWN 14

#define STATE_GOING_DOWN 2
#define STATE_GOING_UP 1
#define STATE_IDLE 0

int state;

// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2

// Prototypes
void sendMessage();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();


Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
String upDone= "UPDONE";
String downDone= "DOWNDONE";
bool calc_delay = false;
SimpleList<uint32_t> nodes;

Task taskSendMessage( TASK_SECOND * 5, TASK_FOREVER, &sendMessage ); // start with a one second interval

// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION | COMMUNICATION);  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();

  randomSeed(analogRead(A0));

  pinMode(MOTOR_DIR,OUTPUT);
  pinMode(MOTOR_STEP,OUTPUT);
  pinMode(SENS_UP,INPUT);
  pinMode(SENS_DOWN,INPUT);
  state=STATE_IDLE;
  Serial.println("setup ended");
}

void tapparella_update() {
        if(digitalRead(SENS_UP) && state==STATE_GOING_UP){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("UP completed");
        mesh.sendBroadcast(upDone);
      }
      if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("DOWN completed");
        mesh.sendBroadcast(downDone);
      }
}


void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  tapparella_update();
  digitalWrite(LED, !onFlag);
}

void sendMessage() {
  String msg = "IAMTAPPARELLA";
  mesh.sendBroadcast(msg);

}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  Serial.printf("U:%d\n",msg.indexOf(SENS_CMDUP));
  Serial.printf("D:%d\n",msg.indexOf(SENS_CMDDOWN));

  if(msg.indexOf(SENS_CMDDOWN)>=0){
    Serial.print("read DOWN,");
    if(state==STATE_IDLE){
      state=STATE_GOING_DOWN;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 1);
      Serial.println("IDLE -> GOING DOWN");
    }else{
      Serial.println("state was not idle");
    }
  }
  if(msg.indexOf(SENS_CMDUP)>=0){
    Serial.print("read UP,");
    if(state==STATE_IDLE){
      state=STATE_GOING_UP;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 0);
      Serial.println("IDLE -> GOING UP");
    }else{
      Serial.println("state was not idle");
    }
  }
}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  //blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  // Reset blink task
  onFlag = false;
  //blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  //blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);

  nodes = mesh.getNodeList();

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}
