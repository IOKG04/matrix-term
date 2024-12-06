#include "util.h"

#if defined(__unix__)
    #define _POSIX_C_SOURCE 200809L
    #include <stdio.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <time.h>
    #include <unistd.h>
#endif

#if defined(__unix__)
    static struct termios termios_previous;
#endif

// sets w and h to terminal width and height in characters;
// returns 0 on success, 1 on a regular failure, -1 if not supported
int get_term_size(int *restrict w, int *restrict h){
    #if defined(__unix__)
        struct winsize wsize;
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize) < 0){
            return 1;
        }
        *w = wsize.ws_col;
        *h = wsize.ws_row;
        return 0;
    #else
        return -1;
    #endif
}

// suspends calling trhead for ms milliseconds
// returns 0 in success, 1 on regular failure, -1 if not supported
int sleep_ms(int ms){
    #if defined(__unix__)
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000000;
        if(nanosleep(&ts, NULL)){
            return 1;
        }
        return 0;
    #else
        return -1;
    #endif
}

// sets buffering of stdout and stdin;
// 0 disables, 1 enables;
// returns 0 in success, 1 on regular failure, -1 if not supported, on failure stuff may still be changed
int set_io_buffering(_Bool kind){
    #if defined(__unix__)
        if(!kind){ // disable
            if(setvbuf(stdout, NULL, _IONBF, 0)){
                return 1;
            }
            if(tcgetattr(STDIN_FILENO, &termios_previous)){
                return 1;
            }
            struct termios t = termios_previous;
            t.c_lflag     &= ~(ICANON | ECHO);
            t.c_cc[VMIN]   = 1;
            t.c_cc[VTIME]  = 0;
            if(tcsetattr(STDIN_FILENO, TCSANOW, &t)){
                return 1;
            }
            return 0;
        }
        else{ // enable
            if(tcsetattr(STDIN_FILENO, TCSANOW, &termios_previous)){
                return 1;
            }
            return 0;
        }
    #else
        return -1;
    #endif
}
