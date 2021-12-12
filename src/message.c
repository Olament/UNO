#include "message.h"
#include "card.h"
#include "server.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Send a message across a socket with a header that includes the message length.
int send_message(int fd, char *message, size_t message_length) {
    // If the message is NULL, set errno to EINVAL and return an error
    if (message == NULL) {
        errno = EINVAL;
        return -1;
    }

    // First, send the length of the message in a size_t
    size_t len = message_length;
    if (write(fd, &len, sizeof(size_t)) != sizeof(size_t)) {
        // Writing failed, so return an error
        return -1;
    }

    // Now we can send the message. Loop until the entire message has been written.
    size_t bytes_written = 0;
    while (bytes_written < len) {
        // Try to write the entire remaining message
        ssize_t rc = write(fd, message + bytes_written, len - bytes_written);

        // Did write fail? If so, return an error
        if (rc <= 0) return -1;

        // If there was no error, write returned the number of bytes written
        bytes_written += rc;
    }

    return 0;
}

// Receive a message from a socket and return the message string (which must be freed later)
char *receive_message(int fd) {
    // First try to read in the message length
    size_t len;
    if (read(fd, &len, sizeof(size_t)) != sizeof(size_t)) {
        // Reading failed. Return an error
        return NULL;
    }

    // Now make sure the message length is reasonable
    if (len > MAX_MESSAGE_LENGTH) {
        errno = EINVAL;
        return NULL;
    }

    // Allocate space for the message
    char *result = malloc(len + 1);

    // Try to read the message. Loop until the entire message has been read.
    size_t bytes_read = 0;
    while (bytes_read < len) {
        // Try to read the entire remaining message
        ssize_t rc = read(fd, result + bytes_read, len - bytes_read);

        // Did the read fail? If so, return an error
        if (rc <= 0) {
            free(result);
            return NULL;
        }

        // Update the number of bytes read
        bytes_read += rc;
    }

    result[len] = '\0';

    return result;
}

int deserialize_int(char *buffer, int *integer) {
    int number = 0;
    int sign = 1;
    int offset = 0;

    // if the first char is '-' sign
    if (buffer[offset] == '-') {
        sign = -1;
        offset++;
    }

    // until we meet null-terminator
    while (buffer[offset] != '\0') {
        number = (number * 10) + (buffer[offset] - '0');
        offset++;
    }
    *integer = number * sign;

    return offset;
}

int serialize_card(char *buffer, card_t *card) {
    int offset = 0;

    offset += sprintf(buffer + offset, "%d", card->type) + 1;
    offset += sprintf(buffer + offset, "%d", card->color) + 1;
    offset += sprintf(buffer + offset, "%d", card->number) + 1;

    buffer[offset] = '\0';
    return offset;
}

int deserialize_card(char *buffer, card_t **card) {
    int offset = 0;

    (*card) = malloc(sizeof(card_t));
    offset += deserialize_int(buffer + offset, (int *) &(*card)->type) + 1;
    offset += deserialize_int(buffer + offset, (int *) &(*card)->color) + 1;
    offset += deserialize_int(buffer + offset, (int *) &(*card)->number) + 1;

    return offset;
}

int serialize_player_status(char *buffer, player_status_t *player) {
    int offset = 0;

    strcpy(buffer, player->player_name);
    offset += strlen(player->player_name) + 1;
    offset += sprintf(buffer + offset, "%d", player->cards_count) + 1;

    buffer[offset] = '\0';
    return offset;
}

int deserialize_player_status(char *buffer, player_status_t **player) {
    int offset = 0;

    (*player) = malloc(sizeof(player_status_t));
    int name_length = strlen(buffer + offset);
    (*player)->player_name = malloc(sizeof(char) * name_length + 1);
    strcpy((*player)->player_name, buffer);
    offset += name_length + 1;
    offset += deserialize_int(buffer + offset, &((*player)->cards_count)) + 1;

    return offset;
}

int serialize_game_status(char *buffer, game_status_t *status) {
    int offset = 0;

    offset += sprintf(buffer + offset, "%d", status->player_count) + 1;
    offset += sprintf(buffer + offset, "%d", status->current_player) + 1;
    offset += sprintf(buffer + offset, "%d", status->direction) + 1;
    offset += serialize_card(buffer + offset, status->previous_card) + 1;
    for (int i = 0; i < status->player_count; i++) {
        offset += serialize_player_status(buffer + offset, status->players[i]) + 1;
    }

    buffer[offset] = '\0';
    return offset;
}

int deserialize_game_status(char *buffer, game_status_t **status) {
    int offset = 0;

    (*status) = malloc(sizeof(game_status_t));
    offset += deserialize_int(buffer + offset, &(*status)->player_count) + 1;
    offset += deserialize_int(buffer + offset, &(*status)->current_player) + 1;
    offset += deserialize_int(buffer + offset, &(*status)->direction) + 1;
    offset += deserialize_card(buffer + offset, &(*status)->previous_card) + 1;
    (*status)->players = malloc(sizeof(player_status_t *) * (*status)->player_count);
    for (int i = 0; i < (*status)->player_count; i++) {
        offset += deserialize_player_status(buffer + offset, &(*status)->players[i]) + 1;
    }

    return offset;
}

void send_payload(int fd, enum MessageType type, void *payload) {
    char *buffer = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
    int offset = 0;

    // write message type
    offset += sprintf(buffer, "%d", type) + 1;

    switch (type) {
        case NOTIFICATION:
            strcpy(&buffer[offset], payload);
            offset += strlen(payload) + 1;
            break;
        case STATUS:
            offset += serialize_game_status(&buffer[offset], payload) + 1;
            break;
        case CARD:
            offset += serialize_card(&buffer[offset], payload) + 1;
            break;
    }

    // send the message
    send_message(fd, buffer, offset);
    // release buffer
    free(buffer);
}

int receive_payload(int fd, void **payload) {
    char *message = receive_message(fd);
    char *p = message;

    // get the message type
    enum MessageType type;
    p += deserialize_int(message, (int *) &type) + 1;

    // deserialize the payload
    switch (type) {
        case NOTIFICATION:
            (*payload) = malloc(sizeof(char) * strlen(p) + 1);
            strcpy(*payload, p);
            break;
        case STATUS:
            deserialize_game_status(p, (game_status_t **) payload);
            break;
        case CARD:
            deserialize_card(p, (card_t **) payload);
            break;
    }

    free(message);
    return type;
}