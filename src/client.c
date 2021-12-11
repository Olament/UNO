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

bool card_match(card_t* previous, card_t* current) {
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
    send_payload(socket_fd, NOTIFICATION, username);

    // current deck
    card_t player_deck[UNO_DECK_SIZE];
    int deck_size = 0;

    // game status
    game_status_t* status = NULL;

    // game started
    for (;;) {
        void* message = NULL;
        int message_type = receive_payload(socket_fd, &message);
        switch (message_type) {
            case NOTIFICATION: {
                char* notification = (char*) message;
                printf("%s\n", notification);

                if (strcmp(notification, "Your turn!") == 0) {
                    // print out available options
                    printf(" 0: Draw a card\n");
                    for (int i = 0; i < deck_size; i++) {
                        printf("%2d: ", i+1);
                        print_card(&player_deck[i]);
                    }

                    // user selection
                    bool valid_input = true;
                    do {
                        
                        printf("select one of the indices above\ninput: ");
                        // scanf("%c", &index_chr);
                        // // check if the range is valid
                        // if (!isdigit(index_selected)) {
                        //     printf("invalid input!\n");
                        //     continue;
                        // }
                        char input[4] = "";
                        int length,i; 
                        
                        scanf("%s", input);
                        length = strlen (input);
                        for (i=0;i<length; i++){
                            if (!isdigit(input[i])){
                                valid_input = false;
                            }
                        }

                        if (valid_input) {
                            valid_input = false;
                        } else {
                            printf("Entered input is not a number\n");
                            continue;
                        }


                        int index_selected = atoi(input);

                        if (index_selected < 0 || index_selected > deck_size || index_selected > 107) {
                            printf("invalid input!\n");
                            continue;
                        }
                        // check if card to play is valid
                        if (index_selected == 0) {
                            // draw a new card
                            send_payload(socket_fd, NOTIFICATION, "draw a new card");
                            valid_input = true;
                        } else {
                            // play a card
                            card_t selected_card = player_deck[index_selected-1];
                            if (card_match(status->previous_card, &selected_card)) {
                                    valid_input = true;
                                    // send selected card to server
                                    send_payload(socket_fd, CARD, &player_deck[index_selected-1]);
                                    // remove the card from deck
                                    player_deck[index_selected-1] = player_deck[deck_size-1];
                                    deck_size--;
                                } else {
                                    printf("invalid card selection\n");
                                }
                        }
                    } while (!valid_input);
                }
                
                if (strcmp(notification, "Please select a color") == 0) {
                    printf("0: RED\n1: YELLOW\n2: GREEN\n3: BLUE\n");
                    int color_selected = 0;
                    scanf("%d", &color_selected);
                    bool valid_input = false;
                    do {
                        if (color_selected<0 || color_selected>3){
                            printf("Invalid input!");
                            continue;
                        } 
                        valid_input = true;
                        card_t new_card;
                        new_card.color = color_selected;
                        send_payload(socket_fd, CARD, &new_card);
                    } while (!valid_input);

                }

                break;
            }
            case STATUS: {
                status = (game_status_t*) message;
                print_game_status(status);
                break;
            }
            case CARD: {
                card_t* card = (card_t*) message;
                player_deck[deck_size++] = (*card);
                print_card(card);
                break;
            }
        }
        free(message);
    }
}