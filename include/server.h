#ifndef UNO_SERVER_H
#define UNO_SERVER_H

#define UNO_DECK_SIZE 108

typedef struct player_status {
    char* player_name;
    int cards_count;
} player_status_t;

typedef struct game_status {
    int player_count;
    int current_player;
    int direction;
    player_status_t** players;
} game_status_t;

void init_cards(card_t cards[UNO_DECK_SIZE]);

void init_game_status(game_status_t* status, int match_size);

#endif //UNO_SERVER_H
