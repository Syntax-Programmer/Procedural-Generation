#!/bin/bash

CC=gcc
CFLAGS="-fsanitize=address -g -Og -Wall -Wextra -Iinclude -I/usr/include/SDL2 /usr/include/json-c/json.h  -Ofast -lSDL2 -lm"

SRC_DIR="src"
BUILD_DIR="build"

CORE_SRC="${SRC_DIR}/core/game.c ${SRC_DIR}/core/graphics_handler.c ${SRC_DIR}/core/state_handler.c"
UTILS_SRC="${SRC_DIR}/utils/obj.c ${SRC_DIR}/utils/physics.c ${SRC_DIR}/utils/map_handler.c"
MAIN_SRC="${SRC_DIR}/main.c"

OUTPUT="${BUILD_DIR}/Game"

mkdir -p "${BUILD_DIR}"

${CC} ${CFLAGS} ${CORE_SRC} ${UTILS_SRC} ${MAIN_SRC} -o "${OUTPUT}"

if [ $? -eq 0 ]; then
    echo "Compilation successful. Run the game using: ./${OUTPUT}"
else
    echo "Compilation failed"
    exit 1
fi