#include <Arduino.h>

#define MOTOR_STEP 15 //no touch
#define MOTOR_DIR 13

#define SENS_UP 12
#define SENS_DOWN 14

#define SENS_CMDUP "U"
#define SENS_CMDDOWN "D"

#define STATE_GOING_DOWN 2
#define STATE_GOING_UP 1
#define STATE_IDLE 0

int state;
String command;
void setup() {

    Serial.begin(9800);
    pinMode(MOTOR_DIR,OUTPUT);
    pinMode(MOTOR_STEP,OUTPUT);
    pinMode(SENS_UP,INPUT);
    pinMode(SENS_DOWN,INPUT);
    state=STATE_IDLE;
    Serial.println("setup ended");
}

void loop() {
    delay(1000);

    Serial.printf("State:%d\n ",state);
    Serial.printf("Write U or D");
    command= Serial.readString();
    if(command.equals("U")){
      Serial.println("received U" );
    }else if(command.equals("D")){
      Serial.println("received D" );

    }
    if(digitalRead(SENS_UP) && state==STATE_GOING_UP){
      state=STATE_IDLE;
      analogWrite(MOTOR_STEP,0);
      Serial.println("UP completed");
    }
    if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
      state=STATE_IDLE;
      analogWrite(MOTOR_STEP,0);
      Serial.println("DOWN completed");
    }

    if(command.equals(SENS_CMDDOWN) && state==STATE_IDLE){
      state=STATE_GOING_DOWN;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 1);
      Serial.println("read DOWN, going");
    }

    if(command.equals(SENS_CMDUP) && state==STATE_IDLE){
      state=STATE_GOING_UP;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 0);
      Serial.println("read UP, going");
    }

}
