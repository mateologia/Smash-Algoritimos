#!/bin/sh
set -eu

cd "$(dirname "$0")"

if ! command -v pkg-config >/dev/null 2>&1 || ! pkg-config --exists raylib; then
    printf '%s\n' "raylib was not found by pkg-config. Install raylib and ensure its pkg-config file is available." >&2
    exit 1
fi

cc -std=c99 -Wall -Wextra -Wpedantic \
    Smash/main.c Smash/combate.c \
    $(pkg-config --cflags --libs raylib) \
    -o /tmp/smash

exec /tmp/smash
