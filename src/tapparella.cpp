
#include <Arduino.h>
#include <painlessMesh.h>
#include "interface.h"
#include "mesh.h"

#define MOTOR_STEP 15 //verde
#define MOTOR_DIR 13 //viola

#define SENS_UP 12 //arancione
#define SENS_DOWN 14 //giallo

#define STATE_GOING_DOWN 2 //g
#define STATE_GOING_UP 1 //
#define STATE_IDLE 0


// Prototypes
void receivedCallback(uint32_t from, String & msg);

//my Prototypes
void tapparella_update();
void setupMesh();
void sendDone();
void sendBroadcast();


Scheduler     userScheduler; // to control your personal task


Task taskSendMessage( TASK_SECOND * 30, TASK_FOREVER, &sendBroadcast ); // start with a one second interval

uint32_t idBrain=NULL;
int state;

void setup() {
  Serial.begin(9600);
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  setupMesh();
  pinMode(MOTOR_DIR,OUTPUT);
  pinMode(MOTOR_STEP,OUTPUT);
  pinMode(SENS_UP,INPUT);
  pinMode(SENS_DOWN,INPUT);
  state=STATE_IDLE;
  Serial.println("setup ended");
}

void checkMovement() {
        if(!digitalRead(SENS_UP) && state==STATE_GOING_UP){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("UP completed");
        sendDone();
      }
      if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("DOWN completed");
        sendDone();
      }
}

void setupMesh(){
  mesh.setDebugMsgTypes(ERROR |STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  if(state!=STATE_IDLE){
    checkMovement();
  }
}

void sendBroadcast() {
  uint8_t id=23;
  char buf[sizeof(PING)*4+sizeof(TAPPARELLA)*4+1];
  snprintf(buf, sizeof (buf), "%s%s%s", " ",PING,TAPPARELLA_NAME);
  String msg = buf;
  msg[0]=id;
  Serial.printf("Sending ping %s to everyone\n",buf);
  mesh.sendBroadcast(msg);
}

void sendDone(){
  String sndmsg= TAPPARELLADONE;
  mesh.sendSingle(idBrain,sndmsg);
}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(msg.indexOf(SENS_CMDDOWN)>=0){
    Serial.print("Read DOWN");
    if(idBrain==NULL){
      idBrain=from;
    }
    if(state==STATE_IDLE){
      state=STATE_GOING_DOWN;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 1);
      Serial.println("IDLE -> GOING DOWN");
    }else{
      Serial.println("State was not idle, please wait");
    }
  }
  if(msg.indexOf(SENS_CMDUP)>=0){
    Serial.print("Read UP");
    if(idBrain==NULL){
      idBrain=from;
    }
    if(state==STATE_IDLE){
      state=STATE_GOING_UP;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 0);
      Serial.println("IDLE -> GOING UP");
    }else{
      Serial.println("State was not idle, please wait");
    }
  }
}

//*/
