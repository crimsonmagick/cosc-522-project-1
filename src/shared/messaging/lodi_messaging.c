#include "messaging/lodi_messaging.h"
#include <string.h>


int serializePClientToLodiServerRequest(PClientToLodiServer toSerialize, char *serialized) {
  int messageType = toSerialize.messageType;
  int userID = toSerialize.userID;
  int recipientId = toSerialize.recipientID;
  unsigned long timestamp = toSerialize.timestamp;
  unsigned long digitalSig = toSerialize.digitalSig;

  memset(serialized, 0, 32);
  char serialized2[32];
  memcpy(serialized, &messageType, sizeof(messageType));
  memcpy(serialized + 4, &userID, sizeof(userID));
  memcpy(serialized + 8, &recipientId, sizeof(recipientId));
  memcpy(serialized + 16, &timestamp, sizeof(timestamp));
  memcpy(serialized + 24, &digitalSig, sizeof(digitalSig));

  memcpy(&serialized2, &toSerialize, sizeof(toSerialize));
  PClientToLodiServer * deserialized1 = (PClientToLodiServer*) serialized2;
  PClientToLodiServer * deserialized2 = (PClientToLodiServer*) serialized2;
  return 0;
}

int deserializeLodiServerToLodiClientAcksResponse(char *serialized, LodiServerToLodiClientAcks *deserialized) {
  return 0;
}
