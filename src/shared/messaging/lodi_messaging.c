#include "messaging/lodi_messaging.h"
#include <string.h>


int serializePClientToLodiServerRequest(PClientToLodiServer toSerialize, char *serialized) {
  int messageType = toSerialize.messageType;
  int userID = toSerialize.userID;
  int recipientId = toSerialize.recipientID;
  int timestamp = toSerialize.timestamp;
  int digitalSig = toSerialize.digitalSig;

  memset(serialized, 0, 20);
  memcpy(serialized, &messageType, sizeof(messageType));
  memcpy(serialized + sizeof(messageType), &userID, sizeof(userID));
  memcpy(serialized + sizeof(messageType) + sizeof(recipientId), &recipientId, sizeof(recipientId));
  memcpy(serialized + sizeof(messageType) + sizeof(recipientId) + sizeof(timestamp),
         &timestamp, sizeof(timestamp));
  memcpy(serialized + sizeof(messageType) + sizeof(recipientId) + sizeof(timestamp) + sizeof(digitalSig),
         &digitalSig, sizeof(digitalSig));
  return 0;
}

int deserializeLodiServerToLodiClientAcksResponse(char *serialized, LodiServerToLodiClientAcks *deserialized) {
  return 0;
}
