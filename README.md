# matrix-term

> transforms your terminal into matrix code

## Goals

Once completed, this should be able to execute a child program, such as `bash` or another shell,
and then transform that process's output into matrix code so you can flex on your non-programmer friends
by looking even more crazy than you do already :3

Every bit of code that is platform specifc should be in [`util.c`](src/util.c), so that porting this is as easy as possible.
