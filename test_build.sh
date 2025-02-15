#!/usr/bin/env bash

# Detect platform
PLATFORM=$(uname)
echo -e "Detected platform: ${PLATFORM}\n"

# Compiler and flags
CC=gcc
CFLAGS="-fsanitize=address -std=c17 -Wall -Wextra -Iinclude/ -O1 -g" # Remove ASan for shipping.
LDFLAGS="-lm"

# Platform-specific configurations
case "$PLATFORM" in
    "Linux")
        CC=gcc
        LDFLAGS="$LDFLAGS -lSDL2"
        ;;
    "Darwin")
        CC=clang
        LDFLAGS="$LDFLAGS -lSDL2"
        ;;
    "MINGW"*)
        CC=x86_64-w64-mingw32-gcc
        OUTPUT_EXTENSION=".exe"
        LDFLAGS="$LDFLAGS -lmingw32 -lSDL2main -lSDL2"
        ;;
    *)
        echo "Unsupported platform: $PLATFORM"
        exit 1
        ;;
esac

# Directories
SRC_DIR="src"
BUILD_DIR="build"

# Source files
MAIN_SRC=$(find "${SRC_DIR}" -name "*.c")

# Output file
OUTPUT="${BUILD_DIR}/test${OUTPUT_EXTENSION}"

# Create build directory
mkdir -p "${BUILD_DIR}"

# Compile and link
echo -e "\nCompiling and linking for ${PLATFORM}...\n"

echo -e "\nErrors and Warnings:\n"
${CC} ${CFLAGS} ${MAIN_SRC} ${LDFLAGS} -o "${OUTPUT}"

# Check for success
if [ $? -eq 0 ]; then
    echo -e "\nCompilation successful. Run the game using: ./${OUTPUT}\n"
else
    echo -e "\nCompilation failed\n"
    exit 1
fi
