/**
 * Shared Lodi Server+Client functions for serializing/deserializing domain structs in a network-safe way, converting bytes to and from
 * big-endian.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "messaging/lodi_messaging.h"

#include "shared.h"
#include "util/buffers.h"
#include "util/server_configs.h"


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

int serializeClientLodi(PClientToLodiServer *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->recipientID);
  appendUint64(serialized, &offset, toSerialize->timestamp);
  appendUint64(serialized, &offset, toSerialize->digitalSig);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int serializeServerLodi(LodiServerToLodiClientAcks *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int deserializeClientLodi(char *serialized, PClientToLodiServer *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->recipientID = getUint32(serialized, &offset);
  deserialized->timestamp = getUint64(serialized, &offset);
  deserialized->digitalSig = getUint64(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int deserializeServerLodi(char *serialized, LodiServerToLodiClientAcks *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int initLodiClientDomain(DomainServiceHandle **handle) {
  const MessageSerializer outgoing = {
    LODI_CLIENT_REQUEST_SIZE,
    .serializer = (int (*)(void *, char *)) serializeClientLodi
  };
  const MessageDeserializer incoming = {
    LODI_SERVER_RESPONSE_SIZE,
    .deserializer = (int (*)(char *, void *)) deserializeServerLodi
  };
  const DomainServiceOpts options = {
    .localPort = 0,
    .timeoutMs = DEFAULT_TIMEOUT_MS,
    .outgoingSerializer = outgoing,
    .incomingDeserializer = incoming
  };

  DomainServiceHandle *allocatedHandle = NULL;
  if (startService(options, &allocatedHandle) != DOMAIN_SUCCESS) {
    return ERROR;
  }
  *handle = allocatedHandle;
  return SUCCESS;
}

int initLodiServerDomain(DomainServiceHandle **handle) {
  const ServerConfig serverConfig = getServerConfig(LODI);
  const MessageSerializer outgoing = {
    LODI_SERVER_RESPONSE_SIZE,
    .serializer = (int (*)(void *, char *)) serializeServerLodi
  };
  const MessageDeserializer incoming = {
    LODI_CLIENT_REQUEST_SIZE,
    .deserializer = (int (*)(char *, void *)) deserializeClientLodi
  };
  const DomainServiceOpts options = {
    .localPort = serverConfig.port,
    .timeoutMs = 0,
    .outgoingSerializer = outgoing,
    .incomingDeserializer = incoming
  };

  DomainServiceHandle *allocatedHandle = NULL;
  if (startService(options, &allocatedHandle) != DOMAIN_SUCCESS) {
    return ERROR;
  }
  *handle = allocatedHandle;
  return SUCCESS;
}