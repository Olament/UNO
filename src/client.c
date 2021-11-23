#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "card.h"
#include "socket.h"
#include "status.h"
#include "message.h"

// socket buffer
char buffer[MAX_MESSAGE_LENGTH];

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server name> <port> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read command line arguments
    char* server_name = argv[1];
    unsigned short port = atoi(argv[2]);
    char* username = argv[3];

    // Connect to the server
    int socket_fd = socket_connect(server_name, port);
    if (socket_fd == -1) {
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    // send server your username
    send_payload(socket_fd, buffer, NOTIFICATION, username);
    printf("sent\n");

    card_t* card = NULL;
    receive_payload(socket_fd, (void**)&card);
    print_card(card);

    game_status_t* status;
    receive_payload(socket_fd, (void**)&status);
    print_game_status(status);
}