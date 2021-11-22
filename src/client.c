#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "card.h"
#include "socket.h"
#include "message.h"

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
    char buffer[MAX_MESSAGE_LENGTH];
    int offset = 0;
    offset += sprintf(buffer, "%d", NOTIFICATION) + 1;
    strcpy(&buffer[offset], username);
    offset += strlen(username);

    send_message(socket_fd, buffer, offset);
}