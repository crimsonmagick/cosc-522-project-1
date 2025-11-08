/**
* This source file implements the "Lodi Server" functionality:
 *
 * Handles Lodi login:
 *   1)  Authenticates the client's digital signature against the associated publicKey
 *   2)  Performs TFA after the digital signature is validated
 *   3)  Responds to the user with a "success" message if both phases succeed
 **/

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messaging/lodi_messaging.h"
#include "shared.h"
#include "messaging/pke_messaging.h"
#include "messaging/tfa_messaging.h"
#include "util/rsa.h"
#include "util/server_configs.h"

static DomainServiceHandle *pkeDomain = NULL;
static struct sockaddr_in pkServerAddress;
static DomainServiceHandle *lodiDomain = NULL;
static struct sockaddr_in lodiServerAddress;
static DomainServiceHandle *tfaDomain = NULL;
static struct sockaddr_in tfaServerAddress;

int sendPushRequest(const unsigned int userID) {
  const TFAClientOrLodiServerToTFAServer requestMessage = {
    .messageType = requestAuth,
    .userID = userID
  };

  if (toDomainHost(tfaDomain, &requestMessage, &tfaServerAddress) == ERROR) {
    printf("Unable to send push notification, aborting...\n");
    return ERROR;
  }

  TFAServerToLodiServer response;
  struct sockaddr_in recreceiveAddr;
  if (fromDomainHost(tfaDomain, &response, &recreceiveAddr) == ERROR) {
    printf("Unable to receive push notification response, aborting...\n");
    return ERROR;
  }

  printf("Push auth confirmation received! Received: messageType=%u, userID=%u\n", response.messageType,
         response.userID);

  return SUCCESS;
}

int main() {
  initPKEClientDomain(&pkeDomain);
  initLodiServerDomain(&lodiDomain);
  initTFAClientDomain(&tfaDomain, false);
  pkServerAddress = getServerAddr(PK);
  lodiServerAddress = getServerAddr(LODI);
  tfaServerAddress = getServerAddr(TFA);

  while (true) {
    struct sockaddr_in clientAddress;
    PClientToLodiServer receivedMessage;
    int receivedSuccess = fromDomainHost(lodiDomain, &receivedMessage, &clientAddress);

    if (receivedSuccess == ERROR) {
      printf("Failed to handle incoming PClientToLodiServer message.\n");
      continue;
    }

    unsigned int publicKey;
    bool authenticated = false;
    if (getPublicKey(pkeDomain, &pkServerAddress, receivedMessage.userID, &publicKey) == ERROR) {
      printf("Failed to retrieve public key!\n");
    } else {
      const unsigned long decrypted = decryptTimestamp(receivedMessage.digitalSig, publicKey, MODULUS);
      if (decrypted == receivedMessage.timestamp) {
        authenticated = true;
        printf("Decrypted timestamp successfully! timestamp=%lu \n", decrypted);
      } else {
        printf("Failed to decrypt timestamp! timestamp=%lu, decrypted=%lu \n",
               receivedMessage.timestamp, decrypted);
      }
    }

    if (authenticated) {
      printf("Verifying login with TFA...\n");
      if (sendPushRequest(receivedMessage.userID) == ERROR) {
        printf("Failed to authenticate with push confirmation! Continuing without response...\n");
        continue;
      }
    } else {
      printf("Failed to authenticate with public key! Continuing without response...\n");
      continue;
    }

    printf("Validated TFA successfully!\n");

    LodiServerToLodiClientAcks toSendMessage = {
      ackLogin,
      receivedMessage.userID,
    };

    const int sendSuccess = toDomainHost(lodiDomain, &toSendMessage, &clientAddress);
    if (sendSuccess == ERROR) {
      printf("Error while sending Lodi login repsonse.\n");
    }
  }
}
