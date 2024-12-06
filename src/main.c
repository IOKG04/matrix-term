#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int main(int argc, char **argv){
    int w, h;
    if(get_term_size(&w, &h)){
        fprintf(stderr, "Failed to get term size\n");
        exit(EXIT_FAILURE);
    }
    printf("w: %i\nh: %i\n", w, h);

    return 0;
}
