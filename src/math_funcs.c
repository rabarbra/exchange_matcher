unsigned min(unsigned a, unsigned b) {
    if (a < b)
        return a;
    return b;
}

int compare_prices(unsigned a[2], unsigned b[2]) {
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

int is_market_price_better(char side, unsigned order[2], unsigned market[2]) {
    int diff;

    diff = compare_prices(order, market);
    if (diff > 0)
        return ((diff = (side == 'B') ? 1 : -1));
    else if (diff < 0)
        return ((diff = (side == 'S') ? 1 : -1));
    else
        return (0);
}
