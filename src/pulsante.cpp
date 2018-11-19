#include <painlessMesh.h>
#include <Arduino.h>



#define TAPPARELLA "TAPPARELLA"



// Prototypes
void sendMessageTapparella(String msg);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

SimpleList<uint32_t> nodes;


String command;
uint32_t idTapparella=NULL;

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
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

}

void loop() {
  //Serial.printf("Write U or D");
  command=Serial.readString();
  if(command.length()>1){
    sendMessageTapparella(command);
    command="";
  }
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}

void sendMessageTapparella(String msg) {
  //TODO sistemare i nodeID per un log
  //String msg = "Hello from node ";
  //msg += mesh.getNodeId();
  //msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  //mesh.sendBroadcast(msg);
  if(idTapparella!=NULL){
    mesh.sendSingle(idTapparella,msg);
    Serial.printf("Sending message: %s\n", msg.c_str());

  }else{
    Serial.printf("Non ci sono tapparelle\n");

  }

}


void receivedCallback(uint32_t from, String & msg) {
  //Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(msg.indexOf(TAPPARELLA) > 0){
    idTapparella=from;
    Serial.printf("Setted tapparella \n");

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
