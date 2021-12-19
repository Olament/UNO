#ifndef CARD_H_
#define CARD_H_

enum CardType {
    NUMBER,
    SKIP,
    REVERSE,
    DRAW_TWO,
    WILD,
    WILD_DRAW,
};

enum CardColor {
    NO_COLOR,
    RED,
    YELLOW,
    GREEN,
    BLUE,
};

typedef struct card {
    enum CardType type;
    enum CardColor color; // only for NUMBER, SKIP, REVERSE, and DRAW_TWO
    int number;           // only for NUMBER
} card_t;

// Print card information -- used in terminal version.
static void print_card(card_t *card) {
    switch (card->color) {
        case RED:
            printf("RED ");
            break;
        case YELLOW:
            printf("YELLOW ");
            break;
        case GREEN:
            printf("GREEN ");
            break;
        case BLUE:
            printf("BLUE ");
            break;
        case NO_COLOR:
            break;
    }
    switch (card->type) {
        case NUMBER:
            printf("%d\n", card->number);
            break;
        case SKIP:
            printf("SKIP\n");
            break;
        case REVERSE:
            printf("REVERSE\n");
            break;
        case DRAW_TWO:
            printf("DRAW_TWO\n");
            break;
        case WILD:
            printf("WILD\n");
            break;
        case WILD_DRAW:
            printf("WILD_DRAW\n");
            break;
        default:
            break;
    }
}

#endif