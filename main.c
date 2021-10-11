#include "includes/exchange_matcher.h"
#include <getopt.h>

int main(int ac, char **av) {
    FILE *in;
    FILE *out;
    int opt;
    result res;

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
    res = process(in, out);
    fclose(in);
    fclose(out);
    return (res);
}
