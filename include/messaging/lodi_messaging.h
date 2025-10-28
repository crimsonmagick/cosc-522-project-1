#ifndef LODI_LODIMESSAGING_H
#define LODI_LODIMESSAGING_H

#define LODI_CLIENT_REQUEST_SIZE (3 * sizeof(uint32_t) + 2 * sizeof(uint64_t))
#define LODI_SERVER_RESPONSE_SIZE (2 * sizeof(uint32_t))

typedef struct {
  enum { ackLogin } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
} LodiServerToLodiClientAcks;


typedef struct {
  enum { login } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
  unsigned int recipientID; /* message recipient identifier */
  unsigned long timestamp; /* timestamp */
  unsigned long digitalSig; /* encrypted timestamp */
} PClientToLodiServer;

char *serializeLodiServerRequest(const PClientToLodiServer *toSerialize);

PClientToLodiServer *deserializeLodiServerRequest(const char *serialized, size_t size);

char *serializeLodiServerResponse(const LodiServerToLodiClientAcks *toSerialize);

LodiServerToLodiClientAcks *deserializeLodiServerResponse(const char *serialized, const size_t size);
#endif
