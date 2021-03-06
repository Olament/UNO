#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include "card.h"
#include "socket.h"
#include "status.h"
#include "message.h"
#include "ui.h"

// Tells whether you can play card "previous"
// if the card "current" is on top of the deck.
bool card_match(card_t *previous, card_t *current) {
    if (current->type == WILD || current->type == WILD_DRAW) {
        return true;
    }
    if (current->color == previous->color) {
        return true;
    }
    if (current->type != previous->type) {
        return false;
    }
    if (current->type == NUMBER && (previous->number != current->number)) {
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server name> <port> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read command line arguments
    char *server_name = argv[1];
    unsigned short port = atoi(argv[2]);
    char *username = argv[3];

    // check username length
    if (strlen(username) > 10) {
        perror("username should be shorter than 10 characters");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    int socket_fd = socket_connect(server_name, port);
    if (socket_fd == -1) {
        perror("Failed to connect");
        exit(EXIT_FAILURE);
    }

    // send server your username
    send_payload(socket_fd, NOTIFICATION, username);

    // current deck
    card_t **player_deck = malloc(sizeof(card_t *) * UNO_DECK_SIZE);
    int deck_size = 0;

    // init game options
    render_options_t *options = malloc(sizeof(render_options_t));
    options->highlight_card = false;
    options->highlight_card_select = false;
    options->highlight_draw_button = false;
    options->cards = player_deck;
    options->deck_size = 0;
    options->card_index = 0;
    options->message_list = malloc(sizeof(message_list_t));
    message_list_init(options->message_list);

    // start ui
    ui_init();

    // game started
    for (;;) {
        void *message = NULL;
        int message_type = receive_payload(socket_fd, &message);

        // check if socket is closed
        if (message == NULL) {
            break;
        }

        switch (message_type) {
            case NOTIFICATION: {
                char *notification = (char *) message;

                // add it to message box
                message_list_add(options->message_list, notification);
                render_ui(options);

                if (strcmp(notification, "Your turn!") == 0) {
                    // pick a card until we get a valid one
                    int card_index = -1;
                    do {
                        card_index = choose_card_ui(options);
                    } while (card_index != -1 &&
                             !card_match(options->game_status->previous_card, player_deck[card_index]));

                    if (card_index == -1) {
                        // draw a new card
                        send_payload(socket_fd, NOTIFICATION, "draw a new card");
                    } else {
                        // play a card
                        card_t *selected_card = player_deck[card_index];
                        send_payload(socket_fd, CARD, selected_card);

                        // sync status
                        player_deck[card_index] = player_deck[deck_size - 1];
                        deck_size -= 1;
                        options->deck_size -= 1;

                        free(selected_card);
                    }
                }

                if (strcmp(notification, "Please select a color") == 0) {
                    // select a color for WILD card
                    int color_selected = popup_message(options, 4,
                                                       "RED", "YELLOW", "GREEN", "BLUE");
                    // send selection to server
                    card_t new_card;
                    new_card.color = color_selected + 1; // shift by 1 to skip NO_COLOR;
                    send_payload(socket_fd, CARD, &new_card);
                }

                free(message);

                break;
            }
            case STATUS: {
                // free previous game_status
                if (options->game_status != NULL) free(options->game_status);
                options->game_status = (game_status_t *) message;
                break;
            }
            case CARD: {
                player_deck[deck_size] = (card_t *) message;
                deck_size += 1;
                options->deck_size += 1;
                break;
            }
        }

        render_ui(options);
    }

    // clean up
    ui_exit();
    message_list_destroy(options->message_list);
    free(options);
    for (int i = 0; i < deck_size; i++) {
        free(player_deck[i]);
    }
    free(player_deck);
}