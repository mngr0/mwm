#include <Arduino.h>
#include <painlessMesh.h>
//#include <AES.h>
#include "../lib/CustomList.h"
#include "../lib/interface.h"




//#include <WiFi101.h>
#include <TelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "SECRET_SSID";             // your network SSID (name)
char pass[] = "SECRET_PASS";           // your network key

// Initialize Telegram BOT
const char BotToken[] = "SECRET_BOT_TOKEN";
WiFiSSLClient client;
TelegramBot bot (BotToken, client);
String whitelist[]= {"Ossigen0","TODO"};
String comandi[]={"\tapUp","\tapDown"};



// Mesh intantiation
painlessMesh  mesh;
String command;
bool firstRound=true;
bool sendAgainTapparella=true;

CustomList<uint32_t> nodes;

// AES Encryption parameters
//const String                  AES_KEY   = "0123456789010123";
//const unsigned long long int  AES_IV    = 36753562;
//const int                     AES_BITS  = 256;





/*

String AES_encrypt(String plain, String key)
{
  AES aes ;
  aes.set_IV(AES_IV);
  byte * plain_buf = (unsigned char*)plain.c_str();
  byte * key_buf = (unsigned char*)key.c_str();
  // add padding where appropriate
  int cipher_length = (plain.length()+1 < 16) ? 16 : (plain.length()+1) + (16 - (plain.length()+1) % 16);
  byte cipher_buf[cipher_length];
  aes.do_aes_encrypt(plain_buf, plain.length() + 1, cipher_buf, key_buf, AES_BITS);
  String cipher = aes.printToHEXString(cipher_buf, cipher_length);
  //aes.printArray(cipher_buf, false);
  uint16_t plain_size = plain.length();
  char lo = plain_size & 0xFF;
  char hi = plain_size >> 8;
  String size_pad;
  for(uint8_t i=0; i<(4-String(plain.length()).length());i++){
    size_pad +="0";
  }
  size_pad+=String(plain.length());
  cipher+=size_pad;
  //Serial.println(sizeof(cipher_buf)/sizeof(cipher_buf[0]));
  //Serial.println(cipher.length());

  plain_size = cipher.substring(cipher.length()-4).toInt();
  //Serial.println(cipher);
  return cipher;
}
String AES_decrypt(String cipher, String key)
{
  //printHeap();

  AES aes ;
  //Serial.println(cipher);
  aes.set_IV(AES_IV);
  byte cipher_buf[cipher.length()/2-2];
  int j = 0;
  for(int i=0;i<=cipher.length()-6;i+=2){
    //Serial.print(cipher.substring(i,i+2));
    cipher_buf[j] = char(strtoul(cipher.substring(i,i+2).c_str(), NULL, 16));
    //Serial.print(char(cipher_buf[j]));
    j++;
  }
  //printHeap();
  //aes.printArray(cipher_buf, false);
  int plain_size = cipher.substring(cipher.length()-4).toInt();
  //Serial.println(plain_size);
 // byte * cipher_buf = (unsigned char*)cipher.c_str();
  byte * key_buf = (unsigned char*)key.c_str();
  int plain_length = cipher.length();
  byte plain_buf[plain_length];
  aes.do_aes_decrypt(cipher_buf, cipher.length()/2-2, plain_buf, key_buf, AES_BITS);
  String plain = aes.printToString(plain_buf, plain_size);
  //Serial.println(sizeof(plain_buf)/sizeof(plain_buf[0]));
  //Serial.println(plain.length());
  //printHeap();
  return plain;
}
*/

/*****************************************************************************/
/* Computes and returns the set difference between two CustomList<uint32_t>  */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CustomList<uint32_t> getDifference(CustomList<uint32_t> arr1, CustomList<uint32_t> arr2){

  int m = arr1.size();
  int n = arr2.size();
  CustomList<uint32_t> diff;
  int i = 0, j = 0;
  while (i < m && j < n)
  {
    if (arr1[i] < arr2[j])
      diff.push_back(arr1[i++]);
    else if (arr2[j] < arr1[i])
      diff.push_back(arr2[j++]);
    else /* if arr1[i] == arr2[j] */
    {
      j++;//Serial.printf(" %d ", arr2[j++]);
      i++;
    }
  }
  return diff;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*******************************************************************************/
/* Computes and returns the set intersection between two CustomList<uint32_t>  */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CustomList<uint32_t> getIntersection(CustomList<uint32_t> arr1, CustomList<uint32_t> arr2){

  int m = arr1.size();
  int n = arr2.size();
  CustomList<uint32_t> intersect;
  int i = 0, j = 0;
  while (i < m && j < n)
  {
    if (arr1[i] < arr2[j])
      i++;
    else if (arr2[j] < arr1[i])
      j++;
    else /* if arr1[i] == arr2[j] */
    {
      intersect.push_back(arr2[j++]);
      i++;
    }
  }
  return intersect;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/**************************************************************************/
/* Given two lists, before and after a new scan, computes and returns all */
/* nodes that have disconnected since the last scan.                      */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CustomList<uint32_t> getLostConnections(CustomList<uint32_t> nodes, CustomList<uint32_t> old_nodes){

  CustomList<uint32_t> diff = getDifference(old_nodes, nodes);
  CustomList<uint32_t> lostCons = getIntersection(diff, old_nodes);
  return lostCons;

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/**************************************************************************/
/* Given two lists, before and after a new scan, computes and returns all */
/* newly connected nodes.                                                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
CustomList<uint32_t> getNewConnections(CustomList<uint32_t> nodes, CustomList<uint32_t> old_nodes){

  CustomList<uint32_t> diff = getDifference(old_nodes, nodes);
  CustomList<uint32_t> newCons = getIntersection(diff, nodes);
  return newCons;

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void changedConnectionCallback() {

    Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
    CustomList<uint32_t> old_nodes = nodes;
    nodes = mesh.getNodeList();
    nodes.sort();
    Serial.printf("Num nodes: %d\n", nodes.size());

    // Print all available connections to nodes
    Serial.printf("Connection list:");
    CustomList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
        Serial.printf(" %u", *node);
        node++;
    }
    Serial.println();

    // Get and print lost connections to nodes
    CustomList<uint32_t> lostCons = getLostConnections(nodes, old_nodes);
    if(lostCons.size()>0){
      Serial.printf("Lost Connections: ");
      node = lostCons.begin();
      while (node != lostCons.end()) {
          Serial.printf(" %u", *node);
          node++;
      }
      Serial.println();
    }
    lostCons.~CustomList<uint32_t>();

    old_nodes = nodes;
}


/************************************************************************/
/* Currently unused painlessMesh callbacks                              */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}
void delayReceivedCallback(uint32_t from, int32_t delay) {
    Serial.printf("Delay to node %u is %d us\n", from, delay);
}








void receivedCallback( uint32_t from, String &msg ) {
  //String decrypted_msg = AES_decrypt(msg, AES_KEY);
  Serial.printf("startHere: Received from %d msg=",from); Serial.println(msg);
  char contentBuffer[500];
  msg .toCharArray(contentBuffer,500);
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& rootFS2 = jsonBuffer.parseObject(contentBuffer);
  String topic = rootFS2["topic"];
  String payload = rootFS2["payload"];
  Serial.println("Message arrived...");
  if(topic == TAPPARELLA_TOPIC){
      Serial.printf("Setted tapparella \n");
  }
  else if (topic == TAPPARELLA_DONE_TOPIC){
    Serial.printf("Tapparella has completed the");
    //Serial.printf(payload);
    Serial.printf("operation\n");
    sendAgainTapparella=true;
    Serial.printf("Write U or D\n");
  }
}

void setup() {
  Serial.begin(9600);
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  Serial.println("setup mesh ended");
  Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");

}

void sendBroadcast(String topic, String message) {
  DynamicJsonBuffer jsonBufferFS;
  JsonObject& rootFS2 = jsonBufferFS.createObject();
  rootFS2["topic"] = topic;
  rootFS2["payload"] =message; //TODO MIO IP
  String json_msg;
  rootFS2.printTo(json_msg);
  Serial.println("Sending.....");
  //String encrypted_msg = AES_encrypt(json_msg, AES_KEY);
  mesh.sendBroadcast(json_msg);
}


void loop() {
  message m = bot.getUpdates();
  if ( m.chat_id != 0 ){ // Checks if there are some updates
    if (m.sender in whitelist){ //TODO
        if(m.text==comandi[0]){ // \tapUp
          if(sendAgainTapparella){
            sendBroadcast(MOVE_TAPPARELLA_TOPIC,SENS_CMDUP);
            sendAgainTapparella=false;
            bot.sendMessage(m.chat_id, "Doing");
          }else{ //tapparella in funzione, aspettare
            bot.sendMessage(m.chat_id, "Still moving, please wait");
          }
        }else if(m.text==comandi[1]){ // \tapDown
          if(sendAgainTapparella){
            sendBroadcast(MOVE_TAPPARELLA_TOPIC,SENS_CMDDOWN);
            sendAgainTapparella=false;
            bot.sendMessage(m.chat_id, "Doing");
          }else{//tapparella in funzione, aspettare
            bot.sendMessage(m.chat_id, "Still moving, please wait");
          }
        }else{ // lista comandi
          bot.sendMessage(m.chat_id, "These are the available commands");
          for(String x in comandi){ //TODO
              bot.sendMessage(m.chat_id, x);
          }
        }

    }

  }

}
