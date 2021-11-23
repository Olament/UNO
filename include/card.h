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

static void print_card(card_t* card) {
    if (card->type == WILD_DRAW) {
        printf("WILD_DRAW\n");
        return;
    }
    if (card->type == WILD) {
        printf("WILD\n");
        return;
    }
    switch (card->color) {
        case RED:
            printf("RED "); break;
        case YELLOW:
            printf("YELLOW "); break;
        case GREEN:
            printf("GREEN "); break;
        case BLUE:
            printf("BLUE "); break;
    }
    switch (card->type) {
        case NUMBER:
            printf("%d\n", card->number); break;
        case SKIP:
            printf("SKIP\n"); break;
        case REVERSE:
            printf("REVERSE\n"); break;
        case DRAW_TWO:
            printf("DRAW_TWO\n"); break;
        default:
            break;
    }
}

#endif