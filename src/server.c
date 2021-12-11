#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "card.h"
#include "message.h"
#include "socket.h"
#include "status.h"

#define DEFAULT_PORT 8887
#define INIT_CARDS_SIZE 7

// socket buffer
char buffer[MAX_MESSAGE_LENGTH];

void notify_all(char* message, int* player_fds, int match_size) {
    for (int i = 0; i < match_size; i++) {
        send_payload(player_fds[i], NOTIFICATION, message);
    }
}

// move the game to next player
void next_player(game_status_t* game_status) {
    game_status->current_player = (game_status->current_player + game_status->direction) 
                % game_status->player_count;
    if (game_status->current_player < 0) {
        game_status->current_player += game_status->player_count;
    }
}

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
    int next_card = 0;

    // init the game status
    game_status_t* game_status = malloc(sizeof(game_status_t));
    init_game_status(game_status, match_size);

    // Open a server socket
    unsigned short port = DEFAULT_PORT;
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

    // have enough player, init the game
    game_status->previous_card->type = NUMBER;
    game_status->previous_card->color = rand() % 4;
    game_status->previous_card->number = rand() % 10;
    for (int i = 0; i < match_size; i++) {
        for (int j = 0; j < INIT_CARDS_SIZE; j++) {
            send_payload(player_fds[i], CARD, &uno_cards[next_card++]);
        }
        game_status->players[i]->cards_count = INIT_CARDS_SIZE;
    }

    
    for (;;) {
        // send the game status to everyone
        int current_fd = player_fds[game_status->current_player];
        for (int i = 0; i < match_size; i++) {
            send_payload(player_fds[i], STATUS, game_status);
        }
        send_payload(current_fd, NOTIFICATION, "Your turn!");

        // wait the current player to response
        void* payload = NULL;
        int type = receive_payload(current_fd, &payload);

        if (type == NOTIFICATION) {
            send_payload(current_fd, CARD, &uno_cards[next_card++]); // eventually check for space
            game_status->players[game_status->current_player]->cards_count++;
        } else if (type == CARD) {
            card_t* card = (card_t*) payload;
            game_status->players[game_status->current_player]->cards_count--;
            game_status->previous_card->type = card->type;
            game_status->previous_card->color = card->color;
            game_status->previous_card->number = card->number;

            switch (card->type) {
                case NUMBER: 
                    break;
                case SKIP:
                    next_player(game_status);
                    current_fd = player_fds[game_status->current_player];
                    send_payload(current_fd, NOTIFICATION, "Your turn was skipped.");
                    break;
                case REVERSE:
                    game_status->direction *= -1;
                    notify_all("The direction has been reversed.", player_fds, match_size);
                    break;
                case DRAW_TWO:
                    next_player(game_status);
                    game_status->players[game_status->current_player]->cards_count += 2;
                    current_fd = player_fds[game_status->current_player];
                    send_payload(current_fd, CARD, &uno_cards[next_card++]);
                    send_payload(current_fd, CARD, &uno_cards[next_card++]);
                    send_payload(current_fd, NOTIFICATION, "You received two cards and your turn was skipped.");
                    break;
                case WILD:
                case WILD_DRAW:
                    send_payload(current_fd, NOTIFICATION, "Please select a color");
                    card_t* wildcard;
                    receive_payload(current_fd, (void**)&wildcard);
                    game_status->previous_card->color = wildcard->color;
                    free(wildcard);
                    if (card->type == WILD_DRAW) {
                        next_player(game_status);
                        game_status->players[game_status->current_player]->cards_count += 4;
                        current_fd = player_fds[game_status->current_player];
                        for (int i = 0; i < 4; i++) {
                            send_payload(current_fd, CARD, &uno_cards[next_card++]);
                        }
                    send_payload(current_fd, NOTIFICATION, "You received four cards and your turn was skipped.");
                    }

                    break;
            }
        }

        // winning check
        if (game_status->players[game_status->current_player]->cards_count == 0) {
            for (int player_index = 0; player_index < game_status->player_count; player_index++) {
                if (player_index == game_status->current_player) {
                    send_payload(player_fds[player_index], NOTIFICATION, "You win!");
                } else {
                    send_payload(player_fds[player_index], NOTIFICATION, "You lose, sorry.");
                }
            }
            return 0;
        }

        next_player(game_status);
    }
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
            cards[index].number = -1;
            index++;
            cards[index] = cards[index-1];
            index++;
        }
    }

    // 4 x WILD and 4 x WILD_DRAW
    for (int i = 0; i < 4; i++) {
        cards[index].number = -1;
        cards[index].color = NO_COLOR;
        cards[index++].type = WILD;
        cards[index].number = -1;
        cards[index].color = NO_COLOR;
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
    status->previous_card = malloc(sizeof(card_t));
    status->players = malloc(sizeof(player_status_t*) * match_size);
    for (int i = 0; i < match_size; i++) {
        status->players[i] = malloc(sizeof(player_status_t));
    }
}