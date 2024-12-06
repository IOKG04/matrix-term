#ifndef UTIL_H__
#define UTIL_H__

// sets w and h to terminal width and height in characters;
// return 0 on success, 1 on a regular failure, -1 if not supported
int get_term_size(int *restrict w, int *restrict h);

#endif
