/*
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <painlessMesh.h>
#include "interface.h"
#include "mesh.h"
#include "ota.h"
//pins
#define MOTOR_STEP 15 //verde
#define MOTOR_DIR 13 //viola

#define SENS_UP 12 //arancione
#define SENS_DOWN 14 //giallo

//possibly states
#define STATE_GOING_DOWN 2
#define STATE_GOING_UP 1
#define STATE_IDLE 0

#define ID_TAPPARELLA 42;
// Prototypes
void receivedCallback(uint32_t from, String & msg);

//my Prototypes
void tapparella_update();
void sendDone();
void sendBroadcast();
void checkAck();
void stopMotor();
Scheduler     userScheduler; // to control your personal task


Task taskSendMessage( TASK_SECOND * 30, TASK_FOREVER, &sendBroadcast ); // start with a one second interval

//brain id
uint32_t idBrain=0;
//state of the tapparella
int state;
//your id


//variables for ack
bool ack=true;
unsigned long timeAnswer;


void setup() {
  Serial.begin(9600);
  //Serial.print(REBUILD);
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  setupMesh();
  Serial.println("------SETUP MESH COMPLETE------");
  setupOTA();
  Serial.println("------SETUP OTA COMPLETE------");
  pinMode(MOTOR_DIR,OUTPUT);
  pinMode(MOTOR_STEP,OUTPUT);
  pinMode(SENS_UP,INPUT);
  pinMode(SENS_DOWN,INPUT);
  state=STATE_IDLE;
  Serial.println("------SETUP PIN COMPLETE------");
  Serial.println("setup ended");
}

void checkMovement() {
      if(!digitalRead(SENS_UP) && state==STATE_GOING_UP){
        Serial.println("Up Completed");
        stopMotor();
        ack=false;
        sendDone();
        timeAnswer=millis();
      }
      if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
        Serial.println("Down Completed");
        stopMotor();
        ack=false;
        sendDone();
        timeAnswer=millis();
      }
}

void stopMotor(){
  state=STATE_IDLE;
  analogWrite(MOTOR_STEP,0);
}


void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  if(state!=STATE_IDLE){
    checkMovement();
  }
  checkAck();
  ArduinoOTA.handle();
}

void checkAck(){
  if(!ack){
    if(millis()>=timeAnswer+REFRESH_RATE){
      Serial.printf("--------RESTARTING--------\n");
      ESP.restart();
    }
  }
}
void sendBroadcast() {
  char buf[sizeof(PING)*4+sizeof(TAPPARELLA_NAME)*4+1];
  snprintf(buf, sizeof (buf), "%s%s%s", " ",PING,TAPPARELLA_NAME);
  String msg = buf;
  msg[0]=ID_TAPPARELLA;
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
    if(idBrain==0){
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
    if(idBrain==0){
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
  if(msg.indexOf(ACK)>=0){
    Serial.print("Read ACK");
    ack=true;
  }
}

//*/
