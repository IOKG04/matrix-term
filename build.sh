#! /bin/sh

cflags="-std=c11 -Wall -Wextra -Wpedantic -Werror"

if [ -z "$1" ] || [ "$1" = "debug" ]; then
    # build in debug mode
    echo "building in debug mode"
    gcc -o matrix-term matrix-term.c $cflags -DDEBUG
elif [ "$1" = "release" ]; then
    # build in release mode
    echo "building in release mode"
    gcc -o matrix-term matrix-term.c $cflags -DNDEBUG -O3
elif [ "$1" = "clean" ]; then
    # delete build related files
    echo "deleting build related files"
    rm -f matrix-term
elif [ "$1" = "help" ]; then
    # help message
    echo "$0 [debug]  | build in debug mode"
    echo "$0 release  | build in release mode"
    echo "$0 clean    | delete build related files"
    echo "$0 help     | show this message"
fi
