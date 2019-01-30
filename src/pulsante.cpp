
#include "interface.h"
#include <Arduino.h>
#include <painlessMesh.h>
#include <TelegramBot.h>
#include "mesh.h"


#define   HOSTNAME         "SmartBrain"
#define   STATION_SSID     "Ossigeno"
#define   STATION_PASSWORD "ciao1234"



// Prototypes
void receivedCallback(uint32_t from, String & msg);

//myfunctions

void setupMesh();
void sendMessageTapparella(String msg,uint8_t index, bool again );
void sendMessageAllTapparelle(String msg );
IPAddress getlocalIP();
void checkCrashTapparelle();
int getIndexTapparella(uint32_t from);
//initialization
IPAddress myIP(0,0,0,0);

//WiFiClient wifiClient;
bool sendAgainTapparelle[64];
uint32_t idTapparelle[64];

unsigned long timeSendTapparella[64];

// Initialize Telegram BOT
const char BotToken[] = "770578956:AAFdTjKvEWId9VIOFKxW23DmPEHOosOoS3c";
//TelegramBot bot (BotToken, mesh);

//code

void setup() {
  Serial.begin(9600);
  setupMesh();
  Serial.println("------SETUP MESH COMPLETE------");
  for (int i=0;i<64;i++){
    sendAgainTapparelle[i]=true;
  }
  //bot.begin();
  //Serial.printf("Bot setted\n");
}

void setupMesh(){
  mesh.setDebugMsgTypes(ERROR |STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
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
    Serial.printf("Write U or D\n");
  }
  //message m = bot.getUpdates();
  String msg=Serial.readString();
  if(msg!=NULL){
    sendMessageAllTapparelle(msg);

  }
  checkCrashTapparelle();
  mesh.update();

}

void checkCrashTapparelle(){
  for(int i=0;i<64;i++){
    if(sendAgainTapparelle[i]==false){
      if((millis())>=(timeSendTapparella[i]+REFRESH_RATE)){
        ESP.restart();
      }
    }
  }
}



void sendMessageAllTapparelle(String msg){
  bool found=false;
  for (uint8_t i=0;i<64;i++){
    if(msg.length()>1 && sendAgainTapparelle[i]){
      if(idTapparelle[i]!=NULL){
        found=true;
        sendMessageTapparella(msg,i,false);
        msg="";
      }
    }
  }
  if(!found){
    Serial.printf("No tapparelle online");
    //ESP.restart(); only to debug
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
  while(index<64 ){
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
		sendAgainTapparelle[index]=true;
	 	Serial.printf("Write U or D\n");
    sendMessageTapparella(ACK, index, true);
  }

}



IPAddress getlocalIP(){
  return IPAddress(mesh.getStationIP());
}
//*/
