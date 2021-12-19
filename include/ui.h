#ifndef UNO_UI_H
#define UNO_UI_H

typedef struct message {
    struct message *next;
    char *message_body;
    time_t message_time;
} message_t;

typedef struct message_list {
    message_t *head;
    message_t *tail;
    int message_count;
} message_list_t;

typedef struct render_options {
    // ui related
    bool highlight_card_select;
    bool highlight_draw_button;
    bool highlight_card;
    // deck info
    card_t **cards;
    int card_index; // card index to highlight
    int deck_size;
    // message list
    message_list_t *message_list;
    // game status
    game_status_t *game_status;
} render_options_t;

// init the message list
void message_list_init(message_list_t *list);

// add a new message to the list with current time
void message_list_add(message_list_t *list, char *message);

// destory the message list data structure
void message_list_destroy(message_list_t *list);

// init the ui element
void ui_init();

// clean up the ui element
void ui_exit();

// re-render the ui using render_options_t
void render_ui(render_options_t *options);

// launch choose_card_ui
// return -1 if user choose to draw a new card, otherwise return the index of the card user wants to play
int choose_card_ui(render_options_t *options);

// show a popup message ui with given options
// return the index of option that user selected
// ex. popup_message(options, 2, "A", "B"), return 1 if user select "B"
int popup_message(render_options_t *options, int nums, ...);

#endif //UNO_UI_H
