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

// Send a message across a socket with a header that includes the message length. Returns non-zero value if
// an error occurs.
int send_message(int fd, char* message, size_t message_size);

// Receive a message from a socket and return the message string (which must be freed later).
// Returns NULL when an error occurs.
char* receive_message(int fd);

int deserialize_int(char* buffer, int* integer);

int serialize_card(char* buffer, card_t* card);

int deserialize_card(char* buffer, card_t* card);

int serialize_player_status(char* buffer, player_status_t* player);

int deserialize_player_status(char* buffer, player_status_t* player);

int serialize_game_status(char* buffer, game_status_t* status);

int deserialize_game_status(char* buffer, game_status_t* status);

#endif //UNO_MESSAGE_H
