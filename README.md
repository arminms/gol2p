[![GitHub License](https://img.shields.io/github/license/arminms/gol2p?logo=github&logoColor=lightgrey&color=yellow)](https://github.com/arminms/gol2p/blob/main/LICENSE)
[![Build and Deploy](https://github.com/arminms/gol2p/actions/workflows/wasm.yml/badge.svg)](https://github.com/arminms/gol2p/actions/workflows/wasm.yml)
[![Build (Linux/macOS/Windows)](https://github.com/arminms/gol2p/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/arminms/gol2p/actions/workflows/cmake-multi-platform.yml)
# gol2p
A C++ implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [SDL2](https://www.libsdl.org/) with support for [WebAssembly (Wasm)](https://en.wikipedia.org/wiki/WebAssembly) via [Emscripten](https://emscripten.org/).

Go to https://arminms.github.io/gol2p to see Wasm version in action.

## To compile and install ``gol2p``
### Desktop (Linux/macOS/Windows)
You need [CMake](https://cmake.org/) version 3.19 or higher:
```bash
cmake -S . -B build && cmake --build build && cmake --install build
```
### Browser (WebAssembly)
You need [Emscripten](https://emscripten.org/):
```bash
em++ src/gol.cpp -o index.html --shell-file src/template.html --use-port=sdl2 -s ALLOW_MEMORY_GROWTH
```
And for running:
```bash
emrun index.html
```