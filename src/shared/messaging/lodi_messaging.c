/**
 * Shared Lodi Server+Client functions for serializing/deserializing domain structs in a network-safe way, converting bytes to and from
 * big-endian.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "messaging/lodi_messaging.h"
#include "util/buffers.h"


char *serializeLodiServerRequest(const PClientToLodiServer *toSerialize) {
  char *serialized = malloc(LODI_CLIENT_REQUEST_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->recipientID);
  appendUint64(serialized, &offset, toSerialize->timestamp);
  appendUint64(serialized, &offset, toSerialize->digitalSig);
  return serialized;
}

PClientToLodiServer *deserializeLodiServerRequest(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < LODI_CLIENT_REQUEST_SIZE) {
    return NULL;
  }

  PClientToLodiServer *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->recipientID = getUint32(serialized, &offset);
  deserialized->timestamp = getUint64(serialized, &offset);
  deserialized->digitalSig = getUint64(serialized, &offset);

  return deserialized;
}

char *serializeLodiServerResponse(const LodiServerToLodiClientAcks *toSerialize) {
  char *serialized = malloc(LODI_SERVER_RESPONSE_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  return serialized;
}

LodiServerToLodiClientAcks *deserializeLodiServerResponse(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < LODI_SERVER_RESPONSE_SIZE) {
    return NULL;
  }

  LodiServerToLodiClientAcks *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);

  return deserialized;
}
