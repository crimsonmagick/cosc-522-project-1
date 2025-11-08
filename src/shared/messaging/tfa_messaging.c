/**
* Shared TFA Server+Client functions for serializing/deserializing domain structs in a network-safe way, converting bytes to and from
 * big-endian.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "messaging/tfa_messaging.h"

#include <stdbool.h>

#include "domain.h"
#include "shared.h"
#include "util/buffers.h"
#include "util/server_configs.h"


int serializeClientTFA(TFAClientOrLodiServerToTFAServer *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint64(serialized, &offset, toSerialize->timestamp);
  appendUint64(serialized, &offset, toSerialize->digitalSig);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int serializeServerTFA(TFAServerToTFAClient *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int deserializeClientTFA(char *serialized, TFAClientOrLodiServerToTFAServer *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->timestamp = getUint64(serialized, &offset);
  deserialized->digitalSig = getUint64(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int deserializeServerTFA(char *serialized, TFAServerToLodiServer *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int initTFAClientDomain(DomainServiceHandle **handle, const bool isDuplex) {
  const MessageSerializer outgoing = {
    TFA_CLIENT_REQUEST_SIZE,
    .serializer = (int (*)(void *, char *))serializeClientTFA
  };
  const MessageDeserializer incoming = {
    TFA_SERVER_RESPONSE_SIZE,
    .deserializer = (int (*)(char *, void *)) deserializeServerTFA
  };
  char * port = NULL;
  if (isDuplex) {
    const ServerConfig server_config = getServerConfig(TFA_CLIENT);
    port = server_config.port;
  }
  const DomainServiceOpts options = {
    .localPort = port,
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

char *serializeTFAServerLodiResponse(const TFAServerToLodiServer *toSerialize) {
  char *serialized = malloc(TFA_SERVER_RESPONSE_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);

  return serialized;
}

TFAServerToLodiServer *deserializeTFAServerLodiResponse(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < TFA_SERVER_RESPONSE_SIZE) {
    return NULL;
  }

  TFAServerToLodiServer *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);

  return deserialized;
}

char *serializeTFAClientRequest(const TFAClientOrLodiServerToTFAServer *toSerialize) {
  char *serialized = malloc(TFA_CLIENT_REQUEST_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint64(serialized, &offset, toSerialize->timestamp);
  appendUint64(serialized, &offset, toSerialize->digitalSig);

  return serialized;
}

TFAClientOrLodiServerToTFAServer *deserializeTFAClientRequest(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < TFA_CLIENT_REQUEST_SIZE) {
    return NULL;
  }

  TFAClientOrLodiServerToTFAServer *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->timestamp = getUint64(serialized, &offset);
  deserialized->digitalSig = getUint64(serialized, &offset);

  return deserialized;
}

char *serializeTFAServerResponse(const TFAServerToTFAClient *toSerialize) {
  char *serialized = malloc(TFA_SERVER_RESPONSE_SIZE);
  if (!serialized) {
    return NULL;
  }

  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);

  return serialized;
}

TFAServerToTFAClient *deserializeTFAServerResponse(const char *serialized, const size_t size) {
  // validate buffer/serialized size
  if (size < TFA_SERVER_RESPONSE_SIZE) {
    return NULL;
  }

  TFAServerToTFAClient *deserialized = malloc(sizeof(*deserialized));
  if (!deserialized) {
    return NULL;
  }

  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);

  return deserialized;
}
