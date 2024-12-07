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
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#if defined(DEBUG)
    #define PRINTERR(msg) fprintf(stderr, msg " at %s, %s, %i\nerrno: %s\n", __FILE__, __func__, __LINE__, strerror(errno))
#elif defined(NDEBUG)
    #define PRINTERR(msg)
#else
    #define PRINTERR(msg) fprintf(stderr, msg "\n")
#endif

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

// pipes
static int child_stdout[2],
           child_stderr[2],
           child_stdin [2];

// entry point (duh)
int main(void){
    int exit_value = EXIT_SUCCESS;
    pid_t is_parent = -1;

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

    // create pipes
    if(pipe2(child_stdout, O_NONBLOCK)){
        PRINTERR("Failed to create child_stdout");
        exit_value = EXIT_FAILURE;
        goto _clean_and_exit;
    }
    if(pipe2(child_stderr, O_NONBLOCK)){
        PRINTERR("Failed to create child_stderr");
        exit_value = EXIT_FAILURE;
        goto _clean_and_exit;
    }
    if(pipe2(child_stdin, O_NONBLOCK)){
        PRINTERR("Failed to create child_stdin");
        exit_value = EXIT_FAILURE;
        goto _clean_and_exit;
    }

    // create child process
    is_parent = fork();
    if(is_parent == -1){
        PRINTERR("Failed to create offspring");
        exit_value = EXIT_FAILURE;
        goto _clean_and_exit;
    }

    // code for the parent
    if(is_parent){
    }

    // code for the child
    else{
        exit_value = EXIT_SUCCESS; // should be the case anyway, and i assume with -O3 gcc knows that too, but just in case

        // redirect stds
        if(dup2(child_stdout[1], STDOUT_FILENO) == -1){
            PRINTERR("Failed to redirect child's stdout");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }
        if(dup2(child_stderr[1], STDERR_FILENO) == -1){
            PRINTERR("Failed to redirect child's stderr");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }
        if(dup2(child_stdin[0], STDIN_FILENO) == -1){
            PRINTERR("Failed to redirect child's stdin");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }

        // close unused pipe ends
        if(close(child_stdout[0])){
            PRINTERR("Failed to close child's unused stdout_pipe end");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }
        if(close(child_stderr[0])){
            PRINTERR("Failed to close child's unused stderr_pipe end");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }
        if(close(child_stdin[1])){
            PRINTERR("Failed to close child's unused stdin_pipe end");
            exit_value = EXIT_FAILURE;
            goto _child_clean_and_exit;
        }

        // TODO: make it execute the new program here

        // exit
       _child_clean_and_exit:
        exit(exit_value);
    }

    // clean and exit
   _clean_and_exit:
    if(is_parent > 0 && kill(is_parent, 0) == 0){
        if(kill(is_parent, SIGKILL)){
            PRINTERR("Failed to kill child");
            exit_value = EXIT_FAILURE;
        }
    }
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