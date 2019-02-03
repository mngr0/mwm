#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_ARRAY 64
#define MAX_COMANDI 10

#define SENS_CMDUP "/Up"
#define SENS_CMDDOWN "/Down"

char* comands[MAX_COMANDI]={SENS_CMDUP,SENS_CMDDOWN};

char * PING = "IAM";
char * TAPPARELLA_NAME = "TAPPARELLA";
char * LUCE_NAME = "LUCE";
char * DONE = "DONE";
char* TAPPARELLADONE= "TAPPARELLADONE";

#define ACK "ACK"


unsigned long REFRESH_RATE= 10000;

#endif
