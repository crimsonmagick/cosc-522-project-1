/**
* Shared PKE Server+Client functions for serializing/deserializing domain structs in a network-safe way, converting bytes to and from
 * big-endian.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "messaging/pke_messaging.h"

#include "domain.h"
#include "shared.h"
#include "util/buffers.h"
#include "util/server_configs.h"

int getPublicKey(DomainServiceHandle * handle, struct sockaddr_in *pkeAddr, const unsigned int userID,
  unsigned int *publicKey) {
  const PClientToPKServer requestMessage = {
    .messageType = requestKey,
    userID
};

  if (toDomainHost(handle, (void *) &requestMessage, pkeAddr) == DOMAIN_FAILURE) {
    printf("Unable to get public key, aborting ...\n");
    return ERROR;
  }

  PKServerToLodiClient responseMessage;
  struct sockaddr_in receivedAddr;
  if (fromDomainHost(handle, &responseMessage, &receivedAddr) == DOMAIN_FAILURE) {
    printf("Failed to receive public key, aborting ...\n");
    return ERROR;
  }

  printf("Received public key successfully! Received: messageType=%u, userID=%u, publicKey=%u\n",
         responseMessage.messageType, responseMessage.userID, responseMessage.publicKey);
  *publicKey = responseMessage.publicKey;

  return SUCCESS;
}

int serializeClientPK(PClientToPKServer *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->publicKey);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int serializeServerPK(PKServerToLodiClient *toSerialize, char *serialized) {
  size_t offset = 0;
  appendUint32(serialized, &offset, toSerialize->messageType);
  appendUint32(serialized, &offset, toSerialize->userID);
  appendUint32(serialized, &offset, toSerialize->publicKey);

  return MESSAGE_SERIALIZER_SUCCESS;
}

int deserializeClientPK(char *serialized, PKServerToLodiClient *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->publicKey = getUint32(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int deserializeServerPK(char *serialized, PKServerToLodiClient *deserialized) {
  size_t offset = 0;
  deserialized->messageType = getUint32(serialized, &offset);
  deserialized->userID = getUint32(serialized, &offset);
  deserialized->publicKey = getUint32(serialized, &offset);

  return MESSAGE_DESERIALIZER_SUCCESS;
}

int initPKEClientDomain(DomainServiceHandle **handle) {
  const MessageSerializer outgoing = {
    PK_CLIENT_REQUEST_SIZE,
    .serializer = (int (*)(void *, char *)) serializeClientPK
  };
  const MessageDeserializer incoming = {
    PK_SERVER_RESPONSE_SIZE,
    .deserializer = (int (*)(char *, void *)) deserializeServerPK
  };
  const DomainServiceOpts options = {
    .localPort = 0,
    .timeoutMs = NULL,
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

int initPKEServerDomain(DomainServiceHandle **handle) {
  const ServerConfig serverConfig = getServerConfig(PK);
  const MessageSerializer outgoing = {
    PK_SERVER_RESPONSE_SIZE,
    .serializer = (int (*)(void *, char *)) serializeServerPK
  };
  const MessageDeserializer incoming = {
    PK_CLIENT_REQUEST_SIZE,
    .deserializer = (int (*)(char *, void *)) deserializeClientPK
  };
  const DomainServiceOpts options = {
    .localPort = serverConfig.port,
    .timeoutMs = NULL,
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
