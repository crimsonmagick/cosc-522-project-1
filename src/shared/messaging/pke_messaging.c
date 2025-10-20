#include <arpa/inet.h>
#include "messaging/pke_messaging.h"

#include <string.h>

//htons - to network short
//htonl - to network long
int serializePKRegistration(PKServerToPClientOrLodiServer toSerialize, char* serialized) {
  int messageType = htonl(toSerialize.messageType);
  int userID = htonl(toSerialize.userID);
  int publicKey = htonl(toSerialize.publicKey);
  memcpy(serialized, &messageType, sizeof(messageType));
  return 0;

}
