# A-RPG (A Role Playing Game)

A simple role-playing game built using SDL2.

## Build the Game

To build the game, run the following command:

```bash
~$ ./make.sh
```

## Run the Game

After building, execute the following command to run the game:

```bash
~$ ./build/Game
```

## Dependencies

This project depends on the Simple DirectMedia Layer 2 (SDL2). Ensure SDL2 is installed on your system before building the game.

### Installing SDL2 on Linux

For Ubuntu/Debian-based systems:

```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

For Fedora-based systems:

```bash
sudo dnf install SDL2-devel SDL2_image-devel SDL2_mixer-devel SDL2_ttf-devel
```

### Installing SDL2 on Windows

To install SDL2 on Windows, follow these steps:

1. **Download SDL2 Libraries**
   - Visit the [SDL2 Downloads page](https://www.libsdl.org/download-2.0.php) and download the "Development Libraries" for Windows (Visual C++).

2. **Extract the Libraries**
   - Extract the downloaded ZIP file to a convenient location (e.g., `C:\SDL2`).

3. **Set Up the Build Environment**
   - Copy the `include` folder from the extracted SDL2 directory to your compiler's include path.
   - Copy the `lib` folder to your compiler's library path.
   - Ensure the `SDL2.dll` file is in the same directory as your compiled game executable or in a directory included in your system's PATH.

4. **Link SDL2 Libraries**
   - When compiling the project, link against the SDL2 libraries (`-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf`).

### Running `make.sh` on Windows

To execute `make.sh` on Windows, follow these steps:

1. **Install a Bash Environment**  
   Download and install a Bash-compatible environment like:
   - [Git for Windows](https://git-scm.com/) (comes with Git Bash).
   - [Cygwin](https://www.cygwin.com/) or [MinGW](http://mingw.org/).
   - [WSL](https://learn.microsoft.com/en-us/windows/wsl/install), if you are using Windows 10/11.

2. **Install `make`**  
   Install a version of `make` for Windows. Here are some options:
   - If using **Git Bash** or **MinGW**, ensure `make` is included in the installation.
   - With **Cygwin**, include `make` during the installation process.
   - For **WSL**, use your Linux distribution's package manager:
     ```bash
     sudo apt install make
     ```

3. **Run the Script**  
   Open the installed Bash terminal (e.g., Git Bash) and run:
   ```bash
   ./make.sh
   ```

By including a Bash-compatible environment and ensuring `make` is installed, you can execute the script and build the game successfully.

## Contributing

Contributions are welcome! If you'd like to contribute to this project, please fork the repository, make your changes, and submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more information.

---

For any issues or suggestions, feel free to open an issue in the repository or contact the developer directly.