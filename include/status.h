#ifndef UNO_STATUS_H
#define UNO_STATUS_H

#include "card.h"

typedef struct player_status {
    char *player_name;
    int cards_count;
} player_status_t;

typedef struct game_status {
    int player_count;
    int current_player;
    int direction;
    card_t *previous_card;
    player_status_t **players;
} game_status_t;

#endif //UNO_STATUS_H
