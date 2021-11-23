#ifndef UNO_SERVER_H
#define UNO_SERVER_H

#include "status.h"

#define UNO_DECK_SIZE 108

void init_cards(card_t cards[UNO_DECK_SIZE]);

void init_game_status(game_status_t* status, int match_size);

#endif //UNO_SERVER_H
