/************************************************\
| anyone is free to do whatever they want with   |
| this software, both in source and binary form. |
| this includes but is not limited to using,     |
| distributing, modifying and sub-licensing it.  |
| including the name of the original author(s)   |
| is encouraged, but not necessary for the above |
| rights.                                        |
| the software is provided "as is", without      |
| warranty of any kind.                          |
\************************************************/

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define PRINTERR(str) fprintf(stderr, str " at %s, %s, %i\n", __FILE__, __func__, __LINE__)

// type of a terminal position
typedef unsigned short term_pos_t;

// suspends calling thread for ms ms
static int sleep_ms(int ms);
// sets buffering of the stds
static int set_io_buffering(int kind);
#define IO_BUF_OFF 0
#define IO_BUF_ON  1

// width and height of terminal (in characters)
static term_pos_t term_w,
                  term_h;

int main(int argc, char **argv){
    int exit_value = EXIT_SUCCESS;

    { // get term_w and -h
        struct winsize wsize;
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize) < 0){
            PRINTERR("Failed to get terminal size");
            exit_value = EXIT_FAILURE;
            goto _clean_and_exit;
        }
        term_w = wsize.ws_col;
        term_h = wsize.ws_row;
    }

    // disable buffering
    if(set_io_buffering(IO_BUF_OFF)){
        PRINTERR("Failed to set io buffering");
        exit_value = EXIT_FAILURE;
        goto _clean_and_exit;
    }

    char lastchar = 0;
    while(lastchar != 'q'){
        ssize_t read_success = read(STDIN_FILENO, &lastchar, 1);
        if(read_success > 0) putchar(lastchar);
        sleep_ms(100);
    }

    // clean and exit
   _clean_and_exit:
    set_io_buffering(IO_BUF_ON);
    exit(exit_value);
}

// suspends calling thread for ms ms
static int sleep_ms(int ms){
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    if(nanosleep(&ts, NULL)){
        PRINTERR("Failed to sleep");
        return 1;
    }
    return 0;
}
// sets buffering of the stds
static int set_io_buffering(int kind){
    static struct termios termios_previous;
    static int stdin_flags;
    static _Bool termios_previous_set = 0,
                 stdin_flags_set      = 0;
    if(kind == IO_BUF_OFF){
        if(setvbuf(stdout, NULL, _IONBF, 0)){
            PRINTERR("Failed to set buffering for stdout");
            return 1;
        }
        if(tcgetattr(STDIN_FILENO, &termios_previous)){
            PRINTERR("Failed to get termios for stdin");
            return 1;
        }
        termios_previous_set = 1;
        struct termios t = termios_previous;
        t.c_lflag     &= ~(ICANON | ECHO);
        t.c_cc[VMIN]   = 1;
        t.c_cc[VTIME]  = 0;
        if(tcsetattr(STDIN_FILENO, TCSANOW, &t)){
            PRINTERR("Failed to set termios for stdin");
            return 1;
        }
        stdin_flags = fcntl(STDIN_FILENO, F_GETFL);
        if(stdin_flags == -1){
            PRINTERR("Failed to get stdin_flags");
            return 1;
        }
        stdin_flags_set = 1;
        if(fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK)){
            PRINTERR("Failed to set stdin flags to be non blocking");
            return 1;
        }
        return 0;
    }
    else if(kind == IO_BUF_ON){
        if(termios_previous_set){
            if(tcsetattr(STDIN_FILENO, TCSANOW, &termios_previous)){
                PRINTERR("Failed to reset termios for stdin");
                return 1;
            }
        }
        if(stdin_flags_set){
            if(fcntl(STDIN_FILENO, F_SETFL, stdin_flags)){
                PRINTERR("Failed to reset stdin flags");
                return 1;
            }
        }
        return 0;
    }
    else{
        PRINTERR("Provided unsupported value");
        return 1;
    }
}
