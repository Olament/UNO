#ifndef UNO_UI_H
#define UNO_UI_H

typedef struct message {
    struct message* next;
    char* message_body;
    time_t message_time;
} message_t;

typedef struct message_list {
    message_t* head;
    message_t* tail;
    int message_count;
} message_list_t;

typedef struct render_options {
    // ui related
    bool highlight_card_select;
    bool highlight_draw_button;
    bool highlight_card;
    // deck info
    card_t** cards;
    int card_index; // card index to highlight
    int deck_size;
    // message list
    message_list_t* message_list;
    // game status
    game_status_t* game_status;
} render_options_t;

void message_list_init(message_list_t* list);

void message_list_add(message_list_t* list, char* message);

void message_list_destroy(message_list_t* list);

void ui_init();

void ui_exit();

void render_ui(render_options_t* options);

int choose_card_ui(render_options_t* options);

int popup_message(render_options_t* options, int nums, ...);

#endif //UNO_UI_H
