#!/usr/bin/env bash
set -e

# Create required data directories
mkdir -p data/bank_data/accounts data/bank_data/transactions data/bank_data/loans

# Check if local source code exists; download from remote only if missing
if [ ! -f "src/main.c" ] || [ ! -f "include/flowcash.h" ]; then
    echo "Local source code not found. Fetching FlowCash source code..."
    mkdir -p src include
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/include/flowcash.h -o include/flowcash.h
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/main.c -o src/main.c
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/admin.c -o src/admin.c
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/user.c -o src/user.c
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/loan.c -o src/loan.c
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/utils.c -o src/utils.c
    curl -sL https://raw.githubusercontent.com/M-Dragonborn/Banking-Project/main/src/demo.c -o src/demo.c
else
    echo "Found local source code."
fi

echo "Compiling FlowCash..."
gcc -Wall -Wextra -std=c11 -Iinclude -o flowcash src/main.c src/admin.c src/user.c src/loan.c src/utils.c src/demo.c

echo "Build complete. Running FlowCash..."
exec ./flowcash "$@"
