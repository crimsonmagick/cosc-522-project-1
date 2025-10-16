#ifndef MESSAGING_H
#define MESSAGING_H

typedef struct {
  enum { registerKey, requestKey } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user's identifier or requested user identifier*/
  unsigned int publicKey; /* user's public key or 0 if message_type is request_key */
} PClientToPKServer;

typedef struct {
  enum { ackRegisterKey, responsePublicKey } messageType; /* same as unsigned int */
  unsigned int userID; /* user identifier or user identifier of requested public key*/
  unsigned int publicKey; /* registered public key or requested public key */
} PKServerToPClientOrLodiServer;

typedef struct {
  enum { login } messageType; /* same size as an unsigned int */
  unsigned int userID; /* user identifier */
  unsigned int recipientID; /* message recipient identifier */
  unsigned long timestamp; /* timestamp */
  unsigned long digitalSig; /* encrypted timestamp */
} PClientToLodiServer;

#endif // MESSAGING_H
