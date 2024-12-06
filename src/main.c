#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

typedef struct{
    short head,
          length;
} matrix_line_t;

static int num_mlines = 0,
           mlines_max = 0;
static matrix_line_t *mlines = NULL;

static _Bool is_over(void){
    for(int i = 0; i < num_mlines; ++i){
        if(mlines[i].head - mlines[i].length < mlines_max) return 0;
    }
    return 1;
}

int main(int argc, char **argv){
    int w, h;
    if(get_term_size(&w, &h)){
        fprintf(stderr, "Failed to get term size in %s, %s, %i\n", __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    num_mlines = w;
    mlines_max = h + 1;

    mlines = malloc(num_mlines * sizeof(*mlines));
    if(!mlines){
        fprintf(stderr, "uwu %i\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    if(set_io_buffering(0)){
        fprintf(stderr, "owo %i\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for(int i = 0; i < num_mlines; ++i){
        mlines[i].head   = -(rand() % (h * 2));
        mlines[i].length = rand() % h;
    }

    printf("\x1b[2J\x1b[?25l");
    while(!is_over()){
        for(int i = 0; i < num_mlines; ++i){
            const short head = (mlines[i].head++),
                        tail = head - mlines[i].length;
            if(head > 0 && head < mlines_max){
                printf("\x1b[%hi;%hiH#", head, i);
            }
            if(tail > 0 && tail < mlines_max){
                printf("\x1b[%hi;%hiH ", tail, i);
            }
        }
        sleep_ms(50);
    }
    printf("\x1b[?25h\x1b[H");

    set_io_buffering(1);
    free(mlines);
    return 0;
}
