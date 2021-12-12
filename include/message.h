#ifndef UNO_MESSAGE_H
#define UNO_MESSAGE_H

#include <stdio.h>

#include "card.h"
#include "server.h"

#define MAX_MESSAGE_LENGTH 2048

enum MessageType {
    NOTIFICATION,
    STATUS,
    CARD,
};

void send_payload(int fd, enum MessageType type, void *payload);

int receive_payload(int fd, void **payload);

#endif //UNO_MESSAGE_H
