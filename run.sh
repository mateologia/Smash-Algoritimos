#!/bin/sh
set -eu

cd "$(dirname "$0")"

USER_RAYLIB_CFLAGS=${RAYLIB_CFLAGS:-}
USER_RAYLIB_LIBS=${RAYLIB_LIBS:-}

case "$(uname -s 2>/dev/null || printf '%s' "${OS:-}")" in
    MINGW*|MSYS*|CYGWIN*|Windows_NT)
        OUTPUT=build/Smash.exe
        PLATFORM_LIBS="-lopengl32 -lgdi32 -lwinmm"
        ;;
    Linux*)
        OUTPUT=build/Smash
        PLATFORM_LIBS="-lGL -lm -lpthread -ldl -lrt -lX11"
        ;;
    Darwin*)
        OUTPUT=build/Smash
        PLATFORM_LIBS="-framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL"
        ;;
    *)
        printf '%s\n' "Unable to detect a supported platform. Run this script from a POSIX shell on Windows, Ubuntu, or macOS." >&2
        exit 1
        ;;
esac

if [ -n "${CC:-}" ]; then
    if ! command -v "$CC" >/dev/null 2>&1; then
        printf 'The configured C compiler was not found: %s\n' "$CC" >&2
        exit 1
    fi
    COMPILER=$CC
else
    COMPILER=
    for candidate in cc gcc clang; do
        if command -v "$candidate" >/dev/null 2>&1; then
            COMPILER=$candidate
            break
        fi
    done
    if [ -z "$COMPILER" ]; then
        printf '%s\n' "No C compiler found. Install cc, gcc, or clang, or set CC to a compiler executable." >&2
        exit 1
    fi
fi

RAYLIB_CFLAGS=
RAYLIB_LIBS="-lraylib"
for pkg_config in pkg-config pkgconf; do
    if command -v "$pkg_config" >/dev/null 2>&1 &&
        "$pkg_config" --exists raylib; then
        RAYLIB_CFLAGS=$("$pkg_config" --cflags raylib)
        RAYLIB_LIBS=$("$pkg_config" --libs raylib)
        break
    fi
done

if [ -z "$RAYLIB_CFLAGS" ] && [ "$RAYLIB_LIBS" = "-lraylib" ] &&
    command -v raylib-config >/dev/null 2>&1; then
    RAYLIB_CFLAGS=$(raylib-config --cflags)
    RAYLIB_LIBS=$(raylib-config --libs)
fi

RAYLIB_CFLAGS=${USER_RAYLIB_CFLAGS:-$RAYLIB_CFLAGS}
RAYLIB_LIBS=${USER_RAYLIB_LIBS:-$RAYLIB_LIBS}

mkdir -p build
set -f
set -- "$COMPILER" -std=c99 -Wall -Wextra -Wpedantic \
    Smash/main.c Smash/combate.c
if [ -n "$RAYLIB_CFLAGS" ]; then
    set -- "$@" $RAYLIB_CFLAGS
fi
set -- "$@" -o "$OUTPUT"
if [ -n "$RAYLIB_LIBS" ]; then
    set -- "$@" $RAYLIB_LIBS
fi
set -- "$@" $PLATFORM_LIBS
"$@"

exec "./$OUTPUT"
