#include <arpa/inet.h>
#include "messaging/pke_messaging.h"

#include <string.h>

//htons - to network short
//htonl - to network long
int serializePKRegistration(PClientToPKServer toSerialize, char *serialized) {
  // int messageType = htonl(toSerialize.messageType);
  // int userID = htonl(toSerialize.userID);
  int messageType = toSerialize.messageType;
  int userID = toSerialize.userID;
  int publicKey = toSerialize.publicKey;
  memset(serialized, 0, 12);
  memcpy(serialized, &messageType, sizeof(messageType));
  memcpy(serialized + sizeof(messageType), &userID, sizeof(userID));
  memcpy(serialized + sizeof(messageType) + sizeof(userID), &publicKey, sizeof(publicKey));
  return 0;
}

int deserializePKRegistration(char *serialized, PKServerToLodiClient *deserialized) {

  int messageType;
  int userID;
  int publicKey;

  memcpy(&messageType, serialized, sizeof(int));
  memcpy(&userID, serialized + sizeof(int), sizeof(int));
  memcpy(&publicKey, serialized + sizeof(int) * 2, sizeof(int));
  deserialized->messageType = messageType;
  deserialized->userID = userID;
  deserialized->publicKey= publicKey;
  return 0;
}
