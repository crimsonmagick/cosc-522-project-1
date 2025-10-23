#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messaging/pke_messaging.h"
#include "util/buffers.h"

char *serializePKClientRequest(const PClientToPKServer *toSerialize) {
  char *serialized = malloc(PK_CLIENT_REQUEST_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->publicKey);

  return serialized;
}

PClientToPKServer *deserializePKClientRequest(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < PK_CLIENT_REQUEST_SIZE) {
    return NULL;
  }

  PClientToPKServer *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->publicKey = getUint32(serialized, &offset);

  return deserialized;
}

char *serializePKServerResponse(const PKServerToLodiClient *toSerialize) {
  char *serialized = malloc(PK_SERVER_RESPONSE_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->publicKey);

  return serialized;
}

PKServerToLodiClient *deserializePKServerResponse(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < PK_SERVER_RESPONSE_SIZE) {
    return NULL;
  }

  PKServerToLodiClient *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->publicKey = getUint32(serialized, &offset);

  return deserialized;
}
