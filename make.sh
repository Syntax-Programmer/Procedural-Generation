#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -Iinclude -I/usr/include/SDL2 -Ofast -lSDL2"

SRC_DIR="src"
BUILD_DIR="build"

CORE_SRC="${SRC_DIR}/core/game.c ${SRC_DIR}/core/player.c ${SRC_DIR}/core/level_parser.c
          ${SRC_DIR}/core/graphics_handler.c ${SRC_DIR}/core/state_handler.c"
ENGINE_SRC="${SRC_DIR}/modules/object.c ${SRC_DIR}/modules/physics.c"
MAIN_SRC="${SRC_DIR}/main.c"

OUTPUT="${BUILD_DIR}/Game"

mkdir -p "${BUILD_DIR}"

${CC} ${CFLAGS} ${CORE_SRC} ${ENGINE_SRC} ${MAIN_SRC} -o "${OUTPUT}"

if [ $? -eq 0 ]; then
    echo "Compilation successful. Run the game using: ./${OUTPUT}"
else
    echo "Compilation failed"
    exit 1
fi
