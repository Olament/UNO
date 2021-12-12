#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <curses.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "card.h"
#include "status.h"
#include "ui.h"

// UI constant
#define PADDING 2
#define SCOREBOARD_HEIGHT 13
#define SCOREBOARD_WIDTH 28
#define MESSAGE_HEIGHT 6
#define CARD_HEIGHT 5
#define CARD_WIDTH  8
#define BUTTON_WIDTH 10

// windows
WINDOW* scoreboard_w;
WINDOW* scoreboard_content_w;
WINDOW* previous_card_w;
WINDOW* message_w;
WINDOW* message_content_w;
WINDOW* card_selection_w;
WINDOW* draw_button_w;
int cards_number; // numbers of card windows
WINDOW** cards_w;

int max(int num1, int num2) {
    return num1 > num2 ? num1 : num2;
}

int min(int num1, int num2) {
    return num1 < num2 ? num1 : num2;
}

void message_list_init(message_list_t* list) {
    list->head = NULL;
    list->tail = NULL;
    list->message_count = 0;
}

// add a new message to message_list by making a copy of message
void message_list_add(message_list_t* list, char* message) {
    // allocating new message
    message_t* new_message = malloc(sizeof(message_t));
    time(&new_message->message_time);
    new_message->message_body = malloc(sizeof(char) * (strlen(message) + 1));
    strcpy(new_message->message_body, message);
    new_message->next = NULL;

    if (list->head == NULL) { // no message inside
        list->head = new_message;
    } else { // has message already
        list->tail->next = new_message;
    }
    list->tail = new_message;
    list->message_count += 1;

    // remove extra message
    while (list->message_count > 5) { // todo: find a better number
        message_t* old_node = list->head;
        list->head = list->head->next;
        free(old_node);
        list->message_count -= 1;
    }
}

void message_list_destroy(message_list_t* list) {
    message_t* curr = list->head;
    while (curr != NULL) {
        message_t* next = curr->next;
        free(curr->message_body);
        free(curr);
        curr = next;
    }
}

void ui_init() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);

    // calculate UI constant and variable
    int max_width, max_height;
    getmaxyx(stdscr, max_height, max_width);

    int start_height = 0;

    // create scoreboard windows
    int scoreboard_height = SCOREBOARD_HEIGHT;
    int scoreboard_width = SCOREBOARD_WIDTH - (2 * PADDING);
    int scoreboard_start_y = 0;
    int scoreboard_start_x = PADDING;

    int scoreboard_content_height = scoreboard_height - 2;
    int scoreboard_content_width = scoreboard_width - 2;
    int scoreboard_content_start_y = scoreboard_start_y + 1;
    int scoreboard_content_start_x = scoreboard_start_x + 1;

    scoreboard_w = newwin(scoreboard_height, scoreboard_width,
                          scoreboard_start_y, scoreboard_start_x);
    scoreboard_content_w = newwin(scoreboard_content_height, scoreboard_content_width,
                                  scoreboard_content_start_y, scoreboard_content_start_x);

    int previous_card_start_y = (scoreboard_height - CARD_HEIGHT) / 2;
    int previous_card_start_x = scoreboard_start_x + scoreboard_width + ((max_width - scoreboard_width - CARD_WIDTH) / 2);

    previous_card_w = newwin(CARD_HEIGHT, CARD_WIDTH,
                             previous_card_start_y, previous_card_start_x);

    start_height += scoreboard_height;

    // create the message box and message content box
    int message_height = MESSAGE_HEIGHT;
    int message_width = max_width - (2 * PADDING);
    int message_start_y = start_height;
    int message_start_x = PADDING;

    int message_content_height = message_height - 2;
    int message_content_width = message_width - (2 * PADDING);
    int message_content_start_y = message_start_y + 1;
    int message_content_start_x = message_start_x + PADDING;

    message_w = newwin(message_height, message_width,
                       message_start_y, message_start_x);
    message_content_w = newwin(message_content_height, message_content_width,
                               message_content_start_y, message_content_start_x);
    scrollok(message_content_w, TRUE);

    start_height += message_height;

    // create card_select windows, draw_button windows, and card windows
    int card_select_height = CARD_HEIGHT + 2 * PADDING;
    int card_select_width = max_width - BUTTON_WIDTH - (3 * PADDING);
    int card_select_start_y = start_height;
    int card_select_start_x = PADDING;

    int button_height = CARD_HEIGHT + 2 * PADDING;
    int button_start_y = card_select_start_y;
    int button_start_x = card_select_width + 2 * PADDING;

    int card_start_y = card_select_start_y + PADDING;
    int card_start_x = card_select_start_x + PADDING;

    cards_number = (card_select_width - PADDING) / (CARD_WIDTH + PADDING);

    card_selection_w = newwin(card_select_height, card_select_width,
                              card_select_start_y, card_select_start_x);
    draw_button_w = newwin(button_height, BUTTON_WIDTH,
                           button_start_y, button_start_x);

    cards_w = malloc(sizeof(WINDOW*) * cards_number);
    for (int i = 0; i < cards_number; i++) {
        cards_w[i] = newwin(CARD_HEIGHT, CARD_WIDTH,
                            card_start_y, card_start_x);
        card_start_x += CARD_WIDTH + PADDING;
    }
}

void ui_exit() {
    delwin(scoreboard_w);
    delwin(scoreboard_content_w);
    delwin(previous_card_w);
    delwin(message_w);
    delwin(message_content_w);
    delwin(card_selection_w);
    delwin(draw_button_w);

    for (int i = 0; i < cards_number; i++) {
        delwin(cards_w[i]);
    }
    free(cards_w);

    endwin();
}

void render_scoreboard(render_options_t* options) {
    if (options->game_status == NULL) return;

    werase(scoreboard_content_w);

    box(scoreboard_w, 0, 0);
    mvwprintw(scoreboard_w, 0, PADDING, "SCOREBOARD");

    wrefresh(scoreboard_w);

    // populate score board
    int player_count = options->game_status->player_count;
    int current_player_index = options->game_status->current_player;
    int direction = options->game_status->direction;
    int next_player_index = ((current_player_index + direction) + player_count) % player_count;

    // scoreboard header
    wprintw(scoreboard_content_w, "         NAME  COUNT\n");

    // scoreboard padding
    for (int i = 0; i < (10 - player_count) / 2; i++) {
        wprintw(scoreboard_content_w, "\n");
    }
    for (int i = 0; i < player_count; i++) {
        wprintw(scoreboard_content_w, "%c%c %10s %6d\n",
                i == current_player_index ? '>' : ' ',
                i == next_player_index ? '*' : ' ',
                options->game_status->players[i]->player_name,
                options->game_status->players[i]->cards_count);
    }

    wrefresh(scoreboard_content_w);
}

void render_card(WINDOW* window, card_t* card, bool is_highlight) {
    werase(window);

    wbkgd(window, COLOR_PAIR(card->color));

    // render highlight
    if (is_highlight) wattron(window, A_STANDOUT);
    box(window, 0, 0);
    if (is_highlight) wattroff(window, A_STANDOUT);

    // render card face
    switch (card->type) {
        case NUMBER:
            mvwprintw(window, 1, 1, "%d", card->number);
            mvwprintw(window, CARD_HEIGHT-2, CARD_WIDTH-2, "%d", card->number);
            break;
        case REVERSE:
            mvwprintw(window, 1, 1, "REV"); break;
        case SKIP:
            mvwprintw(window, 1, 1, "SKIP"); break;
        case DRAW_TWO:
            mvwprintw(window, 1, 1, "DRAW"); break;
        case WILD:
            mvwprintw(window, 1, 1, "WD"); break;
        case WILD_DRAW:
            mvwprintw(window, 1, 1, "WDRAW"); break;
    }

    wrefresh(window);
}

void render_previous_card(render_options_t* options) {
    if (options->game_status == NULL) return;

    render_card(previous_card_w, options->game_status->previous_card, false);
}

//void render_card(render_options_t* options, int windows_index, int card_index) {
//    // erase previous card
//    werase(cards_w[windows_index]);
//
//    // render color
//    wbkgd(cards_w[windows_index], COLOR_PAIR(options->cards[card_index]->color));
//
//    // render highlight
//    if (options->highlight_card && card_index == options->card_index) wattron(cards_w[windows_index], A_STANDOUT);
//    box(cards_w[windows_index], 0, 0);
//    if (options->highlight_card && card_index == options->card_index) wattroff(cards_w[windows_index], A_STANDOUT);
//
//    // render card face
//    mvwprintw(cards_w[windows_index], 1, 1, "%d", options->cards[card_index]->number);
//    mvwprintw(cards_w[windows_index], CARD_HEIGHT-2, CARD_WIDTH-2, "%d", options->cards[card_index]->number);
//
//    wrefresh(cards_w[windows_index]);
//}

void render_card_select(render_options_t* options) {
    // get width and height
    int height, width;
    getmaxyx(card_selection_w, height, width);

    // render card selection windows
    if (options->highlight_card_select) wattron(card_selection_w, A_STANDOUT);
    box(card_selection_w, 0, 0);
    mvwprintw(card_selection_w, 0, PADDING, "CARD SELECTION");
    mvwprintw(card_selection_w, 0, width - 8, "%d/%d",
              (int) ceil((options->card_index + 1) / (double) cards_number),
              (int) ceil(options->deck_size / (double) cards_number));
    if (options->highlight_card_select) wattroff(card_selection_w, A_STANDOUT);

    wrefresh(card_selection_w);

    /* render cards inside */
    int start_index = options->card_index - (options->card_index % cards_number);
    for (int i = start_index; i < min(start_index + cards_number, options->deck_size); i++) {
        int windows_index = i - start_index;
        int card_index = i;
        render_card(cards_w[windows_index], options->cards[card_index],
                    options->highlight_card && card_index == options->card_index);
    }
}

void render_draw_button(render_options_t* options) {
    int width, height;
    getmaxyx(draw_button_w, width, height);

    if (options->highlight_draw_button) wattron(draw_button_w, A_STANDOUT);
    box(draw_button_w, 0, 0);
    mvwprintw(draw_button_w, height / 2, width / 2 - PADDING, "DRAW");
    if (options->highlight_draw_button) wattroff(draw_button_w, A_STANDOUT);

    wrefresh(draw_button_w);
}

void render_message(render_options_t* options) {
    werase(message_w);

    box(message_w, 0, 0);
    mvwprintw(message_w, 0, PADDING, "MESSAGE");
    wrefresh(message_w);

    // populate content windows with message
    message_t* curr = options->message_list->head;
    while (curr != NULL) {
        struct tm* time_info = localtime(&curr->message_time);
        wprintw(message_content_w,
                "%02d:%02d:%02d %s\n", time_info->tm_hour, time_info->tm_min, time_info->tm_sec,
                curr->message_body);
        curr = curr->next;
    }
    wrefresh(message_content_w);
}

void render_ui(render_options_t* options) {
    render_scoreboard(options);
    render_previous_card(options);
    render_message(options);
    render_card_select(options);
    render_draw_button(options);
}

int choose_card_ui(render_options_t* options) {
    // reset and highlight card select by default
    options->card_index = 0;
    options->highlight_card_select = true;
    options->highlight_card = false;
    options->highlight_draw_button = false;

    // render the init state
    refresh();
    render_ui(options);

    int ch;
    while ((ch = wgetch(stdscr))) {
        switch (ch) {
            case 'a':
            case 'A':
                if (options->highlight_draw_button) {
                    options->highlight_draw_button = false;
                    options->highlight_card_select = true;
                }
                if (options->highlight_card) {
                    options->card_index = max(0, options->card_index - 1);
                }
                break;
            case 'd':
            case 'D':
                if (options->highlight_card_select) {
                    options->highlight_card_select = false;
                    options->highlight_draw_button = true;
                }
                if (options->highlight_card) {
                    options->card_index = min(options->deck_size-1, options->card_index + 1);
                }
                break;
            case '\n':
                // get the result and return it
                if (options->highlight_card || options->highlight_draw_button) {
                    int return_value = options->highlight_draw_button ? -1 : options->card_index;

                    // de-highlight everything and return
                    options->card_index = 0;
                    options->highlight_card_select = false;
                    options->highlight_card = false;
                    options->highlight_draw_button = false;
                    render_ui(options);

                    return return_value;
                }
                if (options->highlight_card_select) {
                    options->highlight_card_select = false;
                    options->highlight_card = true;
                }
                break;
            case ' ':
                if (options->highlight_card) {
                    options->highlight_card_select = true;
                    options->highlight_card = false;
                    options->card_index = 0;
                }
                break;
        }
        render_ui(options);
    }

    return -1;
}

int popup_message(render_options_t* options, int nums, ...) {
    va_list va_list1, va_list2;
    va_start(va_list1, nums);
    va_copy(va_list2, va_list1);

    int max_length = 0;
    char** menu_options = malloc(sizeof(char*) * nums);
    for (int i = 0; i < nums; i++) {
        menu_options[i] = malloc(sizeof(char) * (strlen(va_arg(va_list1, char*) + 1)));
        strcpy(menu_options[i], va_arg(va_list2, char*));
        max_length = max(max_length, strlen(menu_options[i]));
    }
    va_end(va_list1);
    va_end(va_list2);

    int max_width, max_height;
    getmaxyx(stdscr, max_height, max_width);

    int popup_width = max_length + (4 * PADDING);
    int popup_height = nums + 2 + (2 * PADDING);
    int popup_start_y = (max_height - popup_height) / 2;
    int popup_start_x = (max_width - popup_width) / 2;
    WINDOW* popup_w = newwin(popup_height, popup_width,
                             popup_start_y, popup_start_x);

    int highlight_index = 0;
    for (;;) {
        // rendering menu
        for (int i = 0; i < nums; i++) {
            if (i == highlight_index) wattron(popup_w, A_STANDOUT);
            mvwprintw(popup_w, i+1+PADDING, 2 * PADDING, "%s", menu_options[i]);
            if (i == highlight_index) wattroff(popup_w, A_STANDOUT);
        }

        box(popup_w, 0, 0);
        mvwprintw(popup_w, 0, PADDING, "SELECT");

        wrefresh(popup_w);

        // get input
        char ch;
        switch (ch = wgetch(popup_w)) {
            case 'w':
            case 'W':
                highlight_index = ((highlight_index - 1) + nums) % nums;
                break;
            case 's':
            case 'S':
                highlight_index = ((highlight_index + 1) + nums) % nums;
                break;
            case '\n':
                // clean up and return
                werase(popup_w);
                wrefresh(popup_w);
                delwin(popup_w);
                render_ui(options);
                return highlight_index;
        }
    }

    return 0; // this should be unreachable
}


//int main() {
//    ui_init();
//
//    render_options_t* options = malloc(sizeof(render_options_t));
//
//    // init card
//    options->deck_size = 20;
//    options->cards = malloc(sizeof(card_t*) * options->deck_size);
//    for (int i = 0; i < options->deck_size; i++) {
//        options->cards[i] = malloc(sizeof(card_t));
//        options->cards[i]->number = i;
//        options->cards[i]->color= i % 5;
//    }
//
//    // init message
//    options->message_list = malloc(sizeof(message_list_t));
//    message_list_init(options->message_list);
//    for (int i = 0; i < 10; i++) {
//        char* temp_message = malloc(sizeof(char) * 20);
//        sprintf(temp_message, "message_%d", i);
//        message_list_add(options->message_list, temp_message);
//        free(temp_message);
//    }
//
//    // init game_status
//    char* random_names[10] = {
//            "Agnes",
//            "Tyriq",
//            "Yuvaan",
//            "Rhiana",
//            "Winter",
//            "Levine",
//            "Bateman",
//            "Rowland",
//            "Kenny",
//            "Zixuan",
//    };
//    options->game_status = malloc(sizeof(game_status_t));
//    options->game_status->current_player = 0;
//    options->game_status->direction = -1;
//    options->game_status->player_count = 10;
//    options->game_status->players = malloc(sizeof(player_status_t*) * options->game_status->player_count);
//    for (int i = 0; i < options->game_status->player_count; i++) {
//        options->game_status->players[i] = malloc(sizeof(player_status_t));
//        options->game_status->players[i]->player_name = random_names[i];
//        options->game_status->players[i]->cards_count = rand() % 20;
//    }
//
//    options->game_status->previous_card = malloc(sizeof(card_t));
//    options->game_status->previous_card->number = 1;
//    options->game_status->previous_card->color = RED;
//
//    render_ui(options);
//
//    int res = popup_message(options, 4,
//                            "RED", "YELLOW", "GREEN", "BLUE");
//    wprintw(stdscr, "%d\n", res);
//    refresh();
//    sleep(2);
//
//    int max_width, max_height;
//    getmaxyx(stdscr, max_height, max_width);
//
//    int pick = choose_card_ui(options);
//    wprintw(stdscr, "%d\n", pick);
//    wprintw(stdscr, "%d %d", max_width, max_height);
//    wrefresh(stdscr);
//
//    sleep(2);
//
//    ui_exit();
//}