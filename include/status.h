#ifndef UNO_STATUS_H
#define UNO_STATUS_H

#include "card.h"

typedef struct player_status {
    char* player_name;
    int cards_count;
} player_status_t;

typedef struct game_status {
    int player_count;
    int current_player;
    int direction;
    card_t* previous_card; 
    player_status_t** players;
} game_status_t;

static void print_game_status(game_status_t* status) {
    printf("--------------------------\n");
    printf("Game Status:\n");
    printf("player count: %d current player: %d direction: %s\n",
           status->player_count, status->current_player,
           status->direction == 1 ? "FORWARD" : "BACKWARD");
    printf("--------------------------\n");
    printf("Players:\n");
    for (int i = 0; i < status->player_count; i++) {
        printf("%d: %10s %d\n", i, status->players[i]->player_name, status->players[i]->cards_count);
    }
    printf("--------------------------\n");
    printf("TOP CARD: ");
    print_card(status->previous_card);
}

#endif //UNO_STATUS_H
