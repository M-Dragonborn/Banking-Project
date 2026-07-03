#!/usr/bin/env bash
set -e
# fetch source, compile, and run
curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/flowcash.c \
    -o flowcash.c
gcc -Wall -Wextra -std=c11 -o flowcash flowcash.c
exec ./flowcash "$@"
