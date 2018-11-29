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

// Prototypes
void sendDone();
void sendBroadcast();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();


Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
//String upDone= "UPDONE";
//String downDone= "DOWNDONE";
SimpleList<uint32_t> nodes;

Task taskSendMessage( TASK_SECOND * 30, TASK_FOREVER, &sendBroadcast ); // start with a one second interval

uint32_t idBrain=NULL;

void setup() {
  Serial.begin(9600);


  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION | COMMUNICATION);  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();


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
      //  mesh.sendBroadcast(upDone);
        sendDone();
      }
      if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("DOWN completed");
      //  mesh.sendBroadcast(downDone);
        sendDone();
      }
}


void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  tapparella_update();
}

void sendBroadcast() {
//  String msg = "IAMTAPPARELLA";
  mesh.sendBroadcast(TAPPARELLA);

}
void sendDone(){
  mesh.sendSingle(idBrain,TAPPARELLADONE);

}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(msg.indexOf(SENS_CMDDOWN)>=0){
    Serial.print("read DOWN,");
    idBrain=from;
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
    idBrain=from;
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
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
  nodes = mesh.getNodeList();
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}
