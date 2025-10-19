#ifndef MESSAGING_H
#define MESSAGING_H

typedef struct {
  enum { registerTFA, ackRegTFA, ackPushTFA, requestAuth } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
  unsigned long timestamp; /* timestamp */
  unsigned long digitalSig; /* encrypted timestamp */
} TFAClientOrLodiServerToTFAServer;

typedef struct {
  enum {confirmTFA , pushTFA } messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier*/
} TFAServerToTFAClient;

typedef struct {
  enum {ackLogin} messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
} LodiServerToLodiClientAcks;

typedef struct {
  enum {ackRegisterKey, responsePublicKey} messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier or user identifier of requested public key*/
  unsigned int publicKey; /* registered public key or requested public key */
} PKServerToLodiClient;
typedef PKServerToLodiClient PKServerToPClientOrLodiServer;

typedef struct {
  enum { registerKey, requestKey } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user's identifier or requested user identifier*/
  unsigned int publicKey; /* user's public key or 0 if message_type is request_key */
} PClientToPKServer;

typedef struct {
  enum { login } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
  unsigned int recipientID; /* message recipient identifier */
  unsigned long timestamp; /* timestamp */
  unsigned long digitalSig; /* encrypted timestamp */
} PClientToLodiServer;

#endif // MESSAGING_H
