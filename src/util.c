#include "util.h"

#if defined(__unix__)
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif // defined(__unix__)

// sets w and h to terminal width and height in characters;
// return 0 on success, 1 on a regular failure, -1 if not supported
int get_term_size(int *restrict w, int *restrict h){
    #if defined(__unix__)
        struct winsize wsize;
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize) < 0){
            return 1;
        }
        *w = wsize.ws_col;
        *h = wsize.ws_row;
        return 0;
    #else // defined(__unix__)
        return -1;
    #endif
}
