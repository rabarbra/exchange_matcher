#include "includes/market_depth.h"
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum result {
    SUCCESS = 0,
    NOTHING_TO_DO,
    MEM_ALLOC_FILED,
    WRONG_DATA,
    END_OF_DATA,
    TRADELINE_OVERFLOW
} result;

uint32_t min(unsigned a, unsigned b) {
    if (a < b)
        return a;
    return b;
}

int8_t compare_price(int a[2], int b[2]) {
    if (a[0] > b[0])
        return 1;
    else if (a[0] < b[0])
        return -1;
    else {
        if (a[1] > b[1])
            return 1;
        else if (a[1] < b[1])
            return -1;
        else
            return 0;
    }
}

t_list *new_list(void *data) {
    t_list *list;

    if (!(list = (t_list *)calloc(1, sizeof(t_list))))
        return NULL;
    list->data = data;
    list->next = NULL;
    return (list);
}

result append_item(t_list *list, void *data) {
    while (list) {
        if (!list->next) {
            list->next = new_list(data);
            if (!list->next)
                return (MEM_ALLOC_FILED);
            return (SUCCESS);
        }
        list = list->next;
    }
    return (WRONG_DATA);
}

void free_list(t_list *list) {
    t_list *buf;

    while (list) {
        buf = list;
        list = buf->next;
        if (buf->data)
            free(buf->data);
        free(buf);
    }
}

result insert_item(t_list **head, t_order *order, char side) {
    t_list *buf;
    t_list *new_item;

    if (!(new_item = new_list(order)))
        return (MEM_ALLOC_FILED);
    buf = *head;
    if (!buf->data) {
        buf->data = new_item;
        return (SUCCESS);
    }
    if ((side == 'B' &&
         compare_price(order->price, ((t_order *)buf->data)->price) > 0) ||
        (side == 'S' &&
         compare_price(order->price, ((t_order *)buf->data)->price) < 0)) {
        new_item->next = buf;
        *head = new_item;
    } else if (side == 'B') {
        while (buf->next && compare_price(((t_order *)buf->next->data)->price,
                                          order->price) >= 0) {
            buf = buf->next;
        }
        new_item->next = buf->next;
        buf->next = new_item;
    } else if (side == 'S') {
        while (buf->next && compare_price(((t_order *)buf->next->data)->price,
                                          order->price) <= 0) {
             buf = buf->next;
        }
        new_item->next = buf->next;
        buf->next = new_item;
    }
    return (SUCCESS);
}

result trade_order(t_list **head, t_order *order, char side, t_list *trades) {
    t_trade *trade;
    t_list *buf = NULL;
    uint32_t volume = 0;
    static uint32_t trade_id = 0;

    volume = min(order->qty, ((t_order *)(*head)->data)->qty);
    order->qty -= volume;
    if (!(trade = (t_trade *)calloc(1, sizeof(t_trade))))
        return (MEM_ALLOC_FILED);
    trade->id = ++trade_id;
    trade->oid1 = ((t_order *)(*head)->data)->oid;
    trade->oid2 = order->oid;
    trade->price[0] = ((t_order *)(*head)->data)->price[0];
    trade->price[1] = ((t_order *)(*head)->data)->price[1];
    trade->qty = volume;
    trade->side = (side == 'B') ? 'S' : 'B';
    append_item(trades, trade);
    if (volume == ((t_order *)(*head)->data)->qty) {
        if ((*head)->next) {
            buf = *head;
            (*head) = buf->next;
            free(buf->data);
            free(buf);
        } else {
            free((*head)->data);
            (*head)->data = NULL;
        }
    }
    return (SUCCESS);
}

result place_order(char side, t_order *order, t_list **bid, t_list **offer,
                   t_list *trades) {
    t_list **place_side;
    t_list **trade_side;

    if (order && bid && offer && trades) {
        if (side == 'B') {
            place_side = bid;
            trade_side = offer;
        } else if (side == 'S') {
            place_side = offer;
            trade_side = bid;
        } else {
            return (WRONG_DATA);
        }
        if (!(*trade_side)->data) {
            if (!(*place_side)->data)
                (*place_side)->data = order;
            else
                insert_item(place_side, order, side);
        } else {
            if ((side == 'B' &&
                 compare_price(order->price,
                               ((t_order *)(*trade_side)->data)->price) < 0) ||
                (side == 'S' &&
                 compare_price(order->price,
                               ((t_order *)(*trade_side)->data)->price) > 0)) {
                insert_item(place_side, order, side);
            } else {
                trade_order(trade_side, order, side, trades);
                if (order->qty)
                    place_order(side, order, bid, offer, trades);
                else
                    free(order);
            }
        }
    } else {
        return (WRONG_DATA);
    }
    return (SUCCESS);
}

result cancel_order(unsigned order_id, t_list **offer, t_list **bid) {
    t_list *buf = NULL;
    t_list *buf_2 = NULL;

    if (offer && *offer && (*offer)->data) {
        buf = *offer;
        if (((t_order *)(*offer)->data)->oid == order_id) {
            (*offer) = buf->next;
            free(buf->data);
            free(buf);
            return (SUCCESS);
        }
        for (; buf->next; buf = buf->next) {
            if (buf->next->data &&
                ((t_order *)buf->next->data)->oid == order_id) {
                buf_2 = buf->next;
                buf->next = buf_2->next;
                free(buf_2->data);
                free(buf_2);
                return (SUCCESS);
            }
        }
    }
    if (bid)
        return (cancel_order(order_id, bid, NULL));
    return (NOTHING_TO_DO);
}

result process(FILE *in, FILE *out) {
    char order_line[ORDER_LINE_SIZE] = {0};
    char trade_line[TRADE_LINE_SIZE] = {0};
    t_order *order = NULL;
    t_list *trades = NULL;
    t_list *trade = NULL;
    t_list *bid = NULL;
    t_list *offer = NULL;
    uint8_t type = 0;
    uint8_t side = 0;
    uint32_t order_id = 0;
    uint8_t ceil_1 = '0';
    uint8_t ceil_2 = '0';

    if (!(trades = new_list(NULL)))
        return (MEM_ALLOC_FILED);
    if (!(bid = new_list(NULL))) {
        free(trades);
        return (MEM_ALLOC_FILED);
    }
    if (!(offer = new_list(NULL))) {
        free(trades);
        free(bid);
        return (MEM_ALLOC_FILED);
    }
    while (fgets(order_line, sizeof(order_line), in)) {
        if (isspace(order_line[0]))
            break;
        if (order_line[0] == 'O') {
            if (!(order = (t_order *)calloc(1, sizeof(t_order)))) {
                free_list(trades);
                free_list(bid);
                free_list(offer);
                return (MEM_ALLOC_FILED);
            }
            if (sscanf(order_line, "%c,%u,%c,%u,%d.%c%c", &type, &order->oid,
                       &side, &order->qty, &order->price[0], &ceil_1,
                       &ceil_2) < 0) {
                free_list(trades);
                free_list(bid);
                free_list(offer);
                free(order);
                return (END_OF_DATA);
            }
            ceil_1 = (ceil_1 > '9' || ceil_1 < '0') ? '0' : ceil_1;
            ceil_2 = (ceil_2 > '9' || ceil_2 < '0') ? '0' : ceil_2;
            order->price[1] = (ceil_1 - 48) * 10 + ceil_2 - 48;
            if (place_order(side, order, &bid, &offer, trades) == SUCCESS) {
                order = NULL;
                trade = trades->next;
                while (trade) {
                    if (snprintf(trade_line, TRADE_LINE_SIZE,
                                "T,%u,%c,%u,%u,%u,%d.%d\n",
                                ((t_trade *)trade->data)->id,
                                ((t_trade *)trade->data)->side,
                                ((t_trade *)trade->data)->oid1,
                                ((t_trade *)trade->data)->oid2,
                                ((t_trade *)trade->data)->qty,
                                ((t_trade *)trade->data)->price[0],
                                ((t_trade *)trade->data)->price[1]) < 0) {
                        free_list(trades);
                        free_list(bid);
                        free_list(offer);
                        return (TRADELINE_OVERFLOW);
                    }
                    fputs(trade_line, out);
                    trade = trade->next;
                }
                free_list(trades->next);
                trades->next = NULL;
            } else {
                free_list(trades);
                free_list(bid);
                free_list(offer);
                return (WRONG_DATA);
            }
        } else if (order_line[0] == 'C') {
            errno = 0;
            order_id = atoi(order_line + 2);
            if (cancel_order(order_id, &offer, &bid) == SUCCESS) {
                sprintf(trade_line, "X,%u\n", order_id);
                fputs(trade_line, out);
            }
            if (errno) {
                free_list(trades);
                free_list(bid);
                free_list(offer);
                return (errno);
            }
        }
    }
    free_list(trades);
    free_list(bid);
    free_list(offer);
    return (SUCCESS);
}

int main(int ac, char **av) {
    FILE *in;
    FILE *out;
    int opt;

    in = stdin;
    out = stdout;
    while ((opt = getopt(ac, av, "i:o:")) != -1) {
        switch (opt) {
        case 'i':
            if ((in = fopen(optarg, "r")) == NULL) {
                fclose(out);
                exit(EXIT_FAILURE);
            }
            break;
        case 'o':
            if ((out = fopen(optarg, "w")) == NULL) {
                fclose(in);
                exit(EXIT_FAILURE);
            };
            break;
        default:
            break;
        }
    }
    process(in, out);
    fclose(in);
    fclose(out);
    return (errno);
}
