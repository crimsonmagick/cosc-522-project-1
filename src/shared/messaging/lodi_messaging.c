#include "messaging/lodi_messaging.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


char *serializePClientToLodiServerRequest(PClientToLodiServer toSerialize) {
    size_t requestSize = sizeof(int32_t) * 3 + sizeof(int64_t) * 2;
    char *serialized = malloc(requestSize);

    memset(serialized + sizeof(int32_t), toSerialize.messageType, sizeof(int32_t));
    memset(serialized + sizeof(int32_t) * 2, toSerialize.messageType, sizeof(int32_t));
    memset(serialized + sizeof(int32_t) * 3, toSerialize.messageType, sizeof(int32_t));
    memset(serialized + sizeof(int32_t) * 3 + sizeof(int64_t) * 1, toSerialize.messageType, sizeof(int32_t));
    memset(serialized + sizeof(int32_t) * 3 + sizeof(int64_t) * 2, toSerialize.messageType, sizeof(int32_t));

    return serialized;
}

int deserializeLodiServerToLodiClientAcksResponse(char *serialized, LodiServerToLodiClientAcks *deserialized) {
    return 0;
}
