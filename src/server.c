#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "card.h"
#include "message.h"
#include "socket.h"
#include "status.h"

// socket buffer
char buffer[MAX_MESSAGE_LENGTH];

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <match_size>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read command line arguments
    int match_size = atoi(argv[1]);

    // file descriptor for each player's socket connection
    int player_fds[match_size];

    card_t uno_cards[UNO_DECK_SIZE];
    init_cards(uno_cards);

    // init the game status
    game_status_t* game_status = malloc(sizeof(game_status_t));
    init_game_status(game_status, match_size);

    // Open a server socket
    unsigned short port = 0;
    int server_socket_fd = server_socket_open(&port);
    if (server_socket_fd == -1) {
        perror("Server socket was not opened");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections, with a maximum of one queued connection
    if (listen(server_socket_fd, match_size)) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %u\n", port);

    // Wait for each player to connect
    for (int i = 0; i < match_size; i++) {
        int client_socket_fd = server_socket_accept(server_socket_fd);
        if (client_socket_fd == -1) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        player_fds[i] = client_socket_fd;
        receive_payload(client_socket_fd, (void**)&game_status->players[i]->player_name);
        printf("player %s connected\n", game_status->players[i]->player_name);
    }

    print_card(&uno_cards[50]);
    send_payload(player_fds[0], buffer, CARD, &uno_cards[50]);

    send_payload(player_fds[0], buffer, STATUS, game_status);
}

void init_cards(card_t cards[UNO_DECK_SIZE]) {
    int index = 0;

    // init the card with color
    for (int color = RED; color <= BLUE; color++) {
        // init the number cards
        // 2 cards for each number except 0 (with only 1)
        for (int number = 0; number < 10; number++) {
            cards[index].color = color;
            cards[index].number = number;
            cards[index].type = NUMBER;
            index++;
            if (number != 0) {
                cards[index] = cards[index-1];
                index++;
            }
        }
        // SKIP, REVERSE, DRAW_TWO
        for (int type = SKIP; type <= DRAW_TWO; type++) {
            cards[index].type = type;
            cards[index].color = color;
            index++;
            cards[index] = cards[index-1];
            index++;
        }
    }

    // 4 x WILD and 4 x WILD_DRAW
    for (int i = 0; i < 4; i++) {
        cards[index++].type = WILD;
        cards[index++].type = WILD_DRAW;
    }

    // shuffle the cards
    srand(time(NULL));
    for (int i = UNO_DECK_SIZE-1; i >= 0; i--) {
        int random_index = rand() % (i+1); // pick a random index between [0, i]
        card_t temp_card = cards[i];
        cards[i] = cards[random_index];
        cards[random_index] = temp_card;
    }
}

void init_game_status(game_status_t* status, int match_size) {
    status->player_count = match_size;
    status->direction = 1;
    status->current_player = 0;
    status->players = malloc(sizeof(player_status_t*) * match_size);
    for (int i = 0; i < match_size; i++) {
        status->players[i] = malloc(sizeof(player_status_t));
    }
}