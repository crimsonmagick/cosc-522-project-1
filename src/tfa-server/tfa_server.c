/**
* This source file implements the "TFA Server" functionality:
 *
 *   1)  Registers client ip addresses and ports
 *   2)  Handles login authentication requests from the Lodi server
 *     i) Interfaces with registered TFA Clients to confirm the push authentication
 **/

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "shared.h"
#include "registration_repository.h"
#include "messaging/tfa_messaging.h"
#include "messaging/pke_messaging.h"
#include "util/rsa.h"
#include "util/server_configs.h"

static DomainServiceHandle *pkeDomain = NULL;
static DomainServiceHandle *tfaDomain = NULL;
static struct sockaddr_in pkServerAddr;

int main() {
    // initialize domains
    initPKEClientDomain(&pkeDomain);
    initTFAServerDomain(&tfaDomain);
    pkServerAddr = getServerAddr(PK);
    // initialize repository
    initRepository();

    while (true) {
        struct sockaddr_in clientAddress;

        TFAClientOrLodiServerToTFAServer receivedMessage;
        if (fromDomainHost(tfaDomain, &receivedMessage, &clientAddress) == DOMAIN_FAILURE) {
            printf("Failed to handle incoming message, continuing...\n");
        }

        if (receivedMessage.messageType == registerTFA) {
            unsigned int publicKey;
            getPublicKey(pkeDomain, &pkServerAddr, receivedMessage.userID, &publicKey);

            // GOT PUBLIC KEY, NOW AUTHENTICATE
            const unsigned long digitalSig = receivedMessage.digitalSig;
            const unsigned long timestamp = receivedMessage.timestamp;
            const unsigned long decryptedTimesstamp = decryptTimestamp(digitalSig, publicKey, MODULUS);
            if (decryptedTimesstamp != timestamp) {
                printf("Authentication failed! Aborting TFA client registration...\n");
                continue;
            }
            printf("Authentication succeeded! Continuing with TFA client registration!\n");

            // WE AUTHENTICATED! SEND SUCCESS MESSAGE TO CLIENT
            TFAServerToTFAClient registrationSuccessMessage = {
                confirmTFA,
                receivedMessage.userID,
            };

            int sendSuccess = toDomainHost(tfaDomain, &registrationSuccessMessage, &clientAddress);

            if (sendSuccess == ERROR) {
                printf("Error while sending message.\n");
                continue;
            }

            // RECEIVE ACK MESSAGE OF DESTINY
            int receivedSuccess = fromDomainHost(tfaDomain, &receivedMessage, &clientAddress);

            if (receivedSuccess == ERROR) {
                printf("Failed to handle incoming ACK TFAClientOrLodiServerToTFAServer message.\n");
                continue;
            }

            if (receivedMessage.messageType != ackRegTFA) {
                printf("Did not receive expected ack register message, aborting registration...\n");
                continue;
            }
            printf("Received expected ack register message! Finishing registration.\n");

            // REGISTER CLIENT
            addIP(receivedMessage.userID, clientAddress.sin_addr, ntohs(clientAddress.sin_port));
            printf("Registered client! Sending TFA confirmation message!\n");
            sendSuccess = toDomainHost(tfaDomain, &registrationSuccessMessage, &clientAddress);
            if (sendSuccess == ERROR) {
                printf("Error while sending final ack message for client registration.\n");
            }
        } else if (receivedMessage.messageType == requestAuth) {
            struct in_addr registeredAddress;
            unsigned short port;
            if (getIP(receivedMessage.userID, &registeredAddress, &port) == ERROR) {
                printf("IP Address was not registered with TFA server! Aborting auth request...\n");
                continue;
            }

            TFAServerToTFAClient pushRequest = {
                .messageType = pushTFA,
                receivedMessage.userID,
            };

            // FIXME we shouldn't be doing pure UDP stuff here...
            struct sockaddr_in tfaClientAddr;
            memset(&tfaClientAddr, 0, sizeof(tfaClientAddr));
            tfaClientAddr.sin_family = AF_INET;
            tfaClientAddr.sin_addr = registeredAddress;
            tfaClientAddr.sin_port = htons(port);

            int sendSuccess = toDomainHost(tfaDomain, &pushRequest, &tfaClientAddr);
            if (sendSuccess == ERROR) {
                printf("Failed to send push auth request to TFA client, aborting...\n");
                continue;
            }

            TFAClientOrLodiServerToTFAServer pushResponse;
            int receivedSuccess = fromDomainHost(tfaDomain, &pushResponse, &tfaClientAddr);
            if (receivedSuccess == ERROR) {
                printf("Error while receiving TFA client push auth message.\n");
                continue;
            }

            if (pushResponse.messageType != ackPushTFA) {
                printf("Did not receive expected ack push message, aborting push auth...\n");
                continue;
            }

            TFAServerToLodiServer pushNotificationResponse = {
                responseAuth,
                receivedMessage.userID
            };
            sendSuccess = toDomainHost(tfaDomain, &pushNotificationResponse, &clientAddress);
            if (sendSuccess == ERROR) {
                printf("Error while sending push response to Lodi server\n");
            }
        }
    }
}
