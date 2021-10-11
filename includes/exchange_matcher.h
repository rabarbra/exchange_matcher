#ifndef EXCHANGE_MATCHER_H
#define EXCHANGE_MATCHER_H
#define ORDER_LINE_SIZE 35
#define TRADE_LINE_SIZE 35
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum result {
    SUCCESS = 0,
    NOTHING_TO_DO,
    MEM_ALLOC_FILED,
    WRONG_DATA,
    END_OF_DATA,
    TRADELINE_OVERFLOW
} result;

typedef struct item {
    void *data;
    struct item *next;
} t_list;

typedef struct {
    unsigned oid;
    unsigned qty;
    unsigned price[2];
} t_order;

typedef struct {
    unsigned id;
    char side;
    unsigned oid1;
    unsigned oid2;
    unsigned qty;
    unsigned price[2];
} t_trade;

t_list *new_list(void *data);
result append_item(t_list *list, void *data);
result insert_item(t_list **head, t_order *order, char side);
void free_list(t_list **list);

result trade_order(t_list **head, t_order *order, char side, t_list *trades);
result place_order(char side, t_order *order, t_list **bid, t_list **offer,
                   t_list *trades);
result cancel_order(unsigned order_id, t_list **offer, t_list **bid);
result process(FILE *in, FILE *out);

unsigned min(unsigned a, unsigned b);
int compare_prices(unsigned a[2], unsigned b[2]);
int is_market_price_better(char side, unsigned order[2], unsigned market[2]);

#endif
