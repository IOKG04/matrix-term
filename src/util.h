#ifndef UTIL_H__
#define UTIL_H__

// sets w and h to terminal width and height in characters;
// returns 0 on success, 1 on a regular failure, -1 if not supported
int get_term_size(int *restrict w, int *restrict h);

// suspends calling trhead for ms milliseconds;
// returns 0 in success, 1 on regular failure, -1 if not supported
int sleep_ms(int ms);

// sets buffering of stdout and stdin;
// 0 disables, 1 enables;
// returns 0 in success, 1 on regular failure, -1 if not supported, on failure stuff may still be changed
int set_io_buffering(_Bool kind);

#endif
