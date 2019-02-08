#include "secrets.h"
#include "interface.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoOTA.h>
//Different for every tapparella
#define ID_TAPPARELLA "1T"

//pins
#define MOTOR_STEP 15 //verde
#define MOTOR_DIR 13 //viola

#define SENS_UP 12 //arancione
#define SENS_DOWN 14 //giallo

#define STATE_GOING_DOWN 2 //g
#define STATE_GOING_UP 1 //
#define STATE_IDLE 0
//IDLE;DOWN;UP
int state;
//if is moving
int inProgress;
long lastMovement;
//Telegram shit
WiFiClientSecure client;
UniversalTelegramBot bot(SECRET_BOT_TOKEN, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
//every user that requested an operation
String users[MAX_ARRAY];


//TODO WHITELIST
void otaSETUP(){
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void wifiSETUP(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void pinsSETUP(){
  pinMode(MOTOR_DIR,OUTPUT);
  pinMode(MOTOR_STEP,OUTPUT);
  pinMode(SENS_UP,INPUT);
  pinMode(SENS_DOWN,INPUT);
  Serial.println("pins OK");
}


void setup() {
  Serial.begin(115200);
  pinsSETUP();
  wifiSETUP();
  otaSETUP();
  state=STATE_IDLE;
}


void setUser(String id){
  for(int i=0;i<MAX_ARRAY;i++){
    if(users[i]==id){
      break;
    }
    if(users[i]==""){
      users[i]=id;
      break;
    }
  }
}

void sendMessageToAllUsers(String msg){
  for(int i=0;i<MAX_ARRAY;i++){
    if(users[i]!=""){
      bot.sendMessage(users[i], msg, "");
    }
  }
}

void movement(int direction){
  if(direction==0){
    if(state==STATE_IDLE){
      state=STATE_GOING_UP;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 0);
      Serial.println("IDLE -> GOING UP");
    }else{
      Serial.println("State was not idle, please wait");
    }

  }else{
    if(state==STATE_IDLE){
      state=STATE_GOING_DOWN;
      analogWrite(MOTOR_STEP,255);
      digitalWrite(MOTOR_DIR, 1);
      Serial.println("IDLE -> GOING DOWN");
    }else{
      Serial.println("State was not idle, please wait");
    }
  }

}


void executeCommand(char* command){
  char str[100];
  if(command==CMDUP){
    movement(0);
    snprintf(str, sizeof str, "%s%s%s", "Tapparella ",ID_TAPPARELLA, " is going up");
    sendMessageToAllUsers(str);
    inProgress=1;
    lastMovement=millis();
  }
  else if (command==CMDDOWN){
    movement(1);
    snprintf(str, sizeof str, "%s%s%s","Tapparella ", ID_TAPPARELLA, "is going down");
    sendMessageToAllUsers(str);
    inProgress=1;
    lastMovement=millis();
  }
}

void telegramHandle(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      int commandPresent=0;
      for (int i=0; i<numNewMessages; i++) {
        String msg=bot.messages[i].text;
        Serial.println(msg);
        setUser(bot.messages[i].chat_id);
        if(msg.indexOf(ID_TAPPARELLA)>=0 || msg.indexOf(SYMBOL_ALL)>=0){
          if(inProgress==0){
            for(int j=0;j<MAX_COMANDI;j++){
              if (msg.indexOf(comandsTapparelle[j])>=0){
                  commandPresent=1;
                  executeCommand(comandsTapparelle[j]);
                  break;
              }
            }
            if(commandPresent==0){
                bot.sendMessage(bot.messages[i].chat_id, "Command not found", "");
            }
          }
          else{
            char str[100];
            snprintf(str, sizeof str, "%s%s%s","Please wait, Tapparella ", ID_TAPPARELLA, "is still going");
            bot.sendMessage(bot.messages[i].chat_id, str, "");

          }
        }

      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}

void movementHandle() {
        char str[100];
        if(!digitalRead(SENS_UP) && state==STATE_GOING_UP){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("UP completed");
        snprintf(str, sizeof str, "%s%s%s","Tapparella ", ID_TAPPARELLA, " up completed");
        sendMessageToAllUsers(str);
        inProgress=0;
      }
      else if(digitalRead(SENS_DOWN) && state==STATE_GOING_DOWN){
        state=STATE_IDLE;
        analogWrite(MOTOR_STEP,0);
        Serial.println("DOWN completed");
        snprintf(str, sizeof str, "%s%s%s","Tapparella ", ID_TAPPARELLA, " down completed");
        sendMessageToAllUsers(str);
        inProgress=0;
      }
}
void crashHandle(){
  if(millis()>=lastMovement+MAX_TIME && inProgress==1){
    Serial.printf("--------RESTARTING--------\n");
      ESP.restart();
  }
}
void loop() {
  if(state!=STATE_IDLE){
    movementHandle();
  }
  ArduinoOTA.handle();
  telegramHandle();
}
