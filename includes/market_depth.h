#ifndef MARKET_DEPTH_H
# define MARKET_DEPTH_H
# define ORDER_LINE_SIZE 25
# define TRADE_LINE_SIZE 25

typedef struct {
    unsigned    oid;
    unsigned    qty;
    int         price[2];
} t_order;

typedef struct {
    unsigned    id;
    char        side;
    unsigned    oid1;
    unsigned    oid2;
    unsigned    qty;
    int         price[2];
} t_trade;

typedef struct item {
    void        *data;
    struct item *next;
} t_list;

typedef struct tree_node {
    struct tree_node    *left;
    struct tree_node    *right;
    t_list              *orders;
} t_tree_node;

#endif
