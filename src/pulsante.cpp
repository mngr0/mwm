#include <Arduino.h>
#include <painlessMesh.h>
#include "interface.h"



// Prototypes
void sendMessageTapparella(String msg,uint8_t index);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

//myfunctions

void setupMesh();
void sendMessageTapparella(String msg,uint8_t index );
void sendMessageAllTapparelle(String msg );
IPAddress getlocalIP();

//initialization
IPAddress myIP(0,0,0,0);
WiFiClient wifiClient;
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;
SimpleList<uint32_t> nodes;
bool sendAgainTapparelle[64];
uint32_t idTapparelle[64];


//code

void setup() {
  Serial.begin(9600);
  setupMesh();
  Serial.println("------SETUP MESH COMPLETE------");
  for (int i=0;i<64;i++){
    sendAgainTapparelle[i]=true;
  }
}

void setupMesh(){
  mesh.setDebugMsgTypes(ERROR |STARTUP | DEBUG | CONNECTION);  // set before init() so that you can see startup messages
  mesh.init( MESH_SSID, MESH_PASSWORD,&userScheduler, MESH_PORT, WIFI_AP_STA, 6 );
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
    Serial.printf("Write U or D");
  }

  String msg=Serial.readString();
  sendMessageAllTapparelle(msg);
  userScheduler.execute();
  mesh.update();
}

void sendMessageAllTapparelle(String msg){
  for (uint8_t i=0;i<64;i++){
    if(msg.length()>1 && sendAgainTapparelle[i]){
      if(idTapparelle[i]!=NULL){
        sendMessageTapparella(msg,i);
        msg="";
        sendAgainTapparelle[i]=false;
      }
    }
  }

}
void sendMessageTapparella(String msg,uint8_t index ) {
  if (idTapparelle[index]!=NULL){
    mesh.sendSingle(idTapparelle[index],msg);
    Serial.printf("Sending message: %s to Tapparella %d \n", msg.c_str(),index);
  }
}



void receivedCallback(uint32_t from, String & msg) {
  int index=0;
  int found=-1;
  while(index<64 ){
    if(idTapparelle[index]==from){
      found=index;
    }
    index++;
  }
  if((msg.indexOf(TAPPARELLA) >= 0) && (found==-1) ){
    idTapparelle[(uint8_t)msg[0]]=from;
    Serial.printf("%d\n",(uint8_t)msg[0]);
    Serial.printf("Setted tapparella \n");
  }
  if((msg.indexOf(TAPPARELLADONE)>=0) && (found!=-1)){
    Serial.printf("Tapparella is done\n");
		sendAgainTapparelle[found]=true;
	 	Serial.printf("Write U or D\n");
  }

}


void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}


void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

IPAddress getlocalIP(){
  return IPAddress(mesh.getStationIP());
}
