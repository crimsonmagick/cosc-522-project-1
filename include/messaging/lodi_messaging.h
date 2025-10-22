#ifndef COSC522_LODI_LODIMESSAGING_H
#define COSC522_LODI_LODIMESSAGING_H

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

char * serializePClientToLodiServerRequest(PClientToLodiServer toSerialize);

int deserializeLodiServerToLodiClientAcksResponse(char *serialized, LodiServerToLodiClientAcks *deserialized);
#endif //COSC522_LODI_LODIMESSAGING_H
