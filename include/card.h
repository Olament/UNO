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

#endif