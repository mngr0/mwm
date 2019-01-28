#ifndef MESH_H
#define MESH_H

//mesh id
#define   MESH_SSID       "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

void newConnectionCallback(uint32_t nodeId);
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);
void changedConnectionCallback();

extern painlessMesh  mesh;

#endif
