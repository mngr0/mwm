
#include "interface.h"
#include "secrets.h"
#include <Arduino.h>
#include <painlessMesh.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "mesh.h"
#include "ota.h"



// Prototypes
void receivedCallback(uint32_t from, String & msg);

//myfunctions

void setupMesh();
void sendMessageTapparella(String msg,uint8_t index, bool again );
void sendMessageAllTapparelle(String msg );
IPAddress getlocalIP();
void checkCrashTapparelle();
int getIndexTapparella(uint32_t from);
void setLord(String id);
void sendMessageToLords(String msg);
void getMessageTelegram();
//initialization
IPAddress myIP(0,0,0,0);


bool sendAgainTapparelle[MAX_ARRAY];
uint32_t idTapparelle[MAX_ARRAY];

unsigned long timeSendTapparella[MAX_ARRAY];

// Initialize Telegram BOT

WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;
String myLords[MAX_ARRAY];
//code











void setLord(String id){
  for(int i=0;i<MAX_ARRAY;i++){
    if(myLords[i]==id){
      break;
    }
    if(myLords[i]==""){
      myLords[i]=id;
      Serial.printf("Found\n");
      break;
    }
  }
}

void sendMessageToLords(String msg){
  for(int i=0;i<MAX_ARRAY;i++){
    if(myLords[i]!=""){
      bot.sendMessage(myLords[i], msg, "");
    }
  }
}
void getMessageTelegram(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Received command");
      for (int i=0; i<numNewMessages; i++) {
        setLord(bot.messages[i].chat_id);
        sendMessageAllTapparelle(bot.messages[i].text);
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
void setup() {
  Serial.begin(9600);
  setupMesh();
  Serial.println("------SETUP MESH COMPLETE------");
  setupOTA();
  Serial.println("------SETUP OTA COMPLETE------");
  for (int i=0;i<MAX_ARRAY;i++){
    sendAgainTapparelle[i]=true;
    myLords[i]="";
  }
}

void setupMesh(){
  mesh.setDebugMsgTypes(ERROR |STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.init( MESH_SSID, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
}


void loop() {
   if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
  getMessageTelegram();
  checkCrashTapparelle();
  mesh.update();
  ArduinoOTA.handle();
}

void checkCrashTapparelle(){
  for(int i=0;i<MAX_ARRAY;i++){
    if(sendAgainTapparelle[i]==false){
      if((millis())>=(timeSendTapparella[i]+REFRESH_RATE)){
      Serial.printf("--------RESTARTING--------\n");
        ESP.restart();
      }
    }
  }
}



void sendMessageAllTapparelle(String msg){
  bool comandFound=false;
  for(uint8_t j=0;j<MAX_COMANDI;j++ ){
    if(msg==comands[j]){
      comandFound=true;
      bool found=false;
      for (uint8_t i=0;i<MAX_ARRAY;i++){
        if(msg.length()>1 && sendAgainTapparelle[i]){
          if(idTapparelle[i]!=NULL){
            found=true;
            sendMessageTapparella(msg,i,false);
            msg="";
          }
        }
      }
      if(!found){
        Serial.printf("No tapparelle online\n");
        sendMessageToLords("No tapparelle online");
        //ESP.restart(); only to debug
      }

    }
  }
  if(!comandFound){
    Serial.printf("Command not valid\n");
    sendMessageToLords("Command not valid");

  }


}
void sendMessageTapparella(String msg,uint8_t index, bool ack ) {
  if (idTapparelle[index]!=NULL){
    mesh.sendSingle(idTapparelle[index],msg);
    if(!ack){
      Serial.printf("Sending message: %s to Tapparella %d \n", msg.c_str(),index);
      sendAgainTapparelle[index]=false;
      timeSendTapparella[index]=millis();
    }
    else{
      Serial.printf("Sending ack: %s to Tapparella %d \n", msg.c_str(),index);

    }
  }
}


int getIndexTapparella(uint32_t from){
  int index=0;
  int found=-1;
  while(index<MAX_ARRAY ){
    if(idTapparelle[index]==from){
      found=index;
    }
    index++;
  }
  return found;
}


void receivedCallback(uint32_t from, String & msg){

  Serial.printf("message %s\n",msg.c_str());
  if((msg.indexOf(PING) >= 0) ){
    if(msg.indexOf(TAPPARELLA_NAME)>=0 ){
      int index=getIndexTapparella(from);
      if(index==-1){
        idTapparelle[(uint8_t)msg[0]]=from;
        Serial.printf("Setted tapparella %d  \n",(uint8_t)msg[0]);
      }
    }

  }
  if((msg.indexOf(TAPPARELLADONE)>=0)){
    int index=getIndexTapparella(from);
    Serial.printf("Tapparella is done\n");
    sendMessageToLords("Tapparelle done");
		sendAgainTapparelle[index]=true;
    sendMessageTapparella(ACK, index, true);
  }

}



IPAddress getlocalIP(){
  return IPAddress(mesh.getStationIP());
}
//*/
