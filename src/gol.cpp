//
// Copyright (c) 2024 Armin Sobhani
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <iostream>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#endif // __EMSCRIPTEN__

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 3;
const int ROWS = WINDOW_HEIGHT / CELL_SIZE;
const int COLS = WINDOW_WIDTH / CELL_SIZE;

uint8_t **grid;
uint8_t **new_grid;

void initialize_grid()
{   std::random_device r;
    std::default_random_engine e(r());
    std::uniform_int_distribution<int> uniform_dist(0, 1);
    grid = new uint8_t*[ROWS];
    new_grid = new uint8_t*[ROWS];
    for (int i = 0; i < ROWS; ++i)
    {   grid[i] = new uint8_t[COLS];
        new_grid[i] = new uint8_t[COLS];
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = uniform_dist(e);
    }
}

int count_live_neighbors(int x, int y)
{   int live_neighbors = 0;
    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
        {   if (i == 0 && j == 0) continue;
            int newX = (x + i + ROWS) % ROWS;
            int newY = (y + j + COLS) % COLS;
            live_neighbors += grid[newX][newY];
        }
    return live_neighbors;
}

void update_grid()
{   for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
        {   int live_neighbors = count_live_neighbors(i, j);
            if (grid[i][j] == 1 && (live_neighbors < 2 || live_neighbors > 3))
                new_grid[i][j] = 0;
            else if (grid[i][j] == 0 && live_neighbors == 3)
                new_grid[i][j] = 1;
            else
                new_grid[i][j] = grid[i][j];
        }
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = new_grid[i][j];
}

void draw_grid(SDL_Renderer* renderer)
{   for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
        {   SDL_Rect cell;
            cell.x = j * CELL_SIZE;
            cell.y = i * CELL_SIZE;
            cell.w = CELL_SIZE;
            cell.h = CELL_SIZE;
            SDL_SetRenderDrawColor
            (   renderer
            ,   grid[i][j] * 0
            ,   grid[i][j] * 255
            ,   grid[i][j] * 0
            ,   255
            );
            SDL_RenderFillRect(renderer, &cell);
        }
}

void main_loop(void* renderer_)
{   SDL_Renderer *renderer = (SDL_Renderer*)renderer_;
    update_grid();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_grid(renderer);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{   SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow
    (   "John Conway's Game of Life"
    ,   SDL_WINDOWPOS_UNDEFINED
    ,   SDL_WINDOWPOS_UNDEFINED
    ,   WINDOW_WIDTH
    ,   WINDOW_HEIGHT
    ,   0
    );
    SDL_Renderer* renderer = SDL_CreateRenderer
    (   window
    ,   -1
    ,   SDL_RENDERER_ACCELERATED
    );

    initialize_grid();

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    std::cout << "John Conway's Game of Life v1.0\n"
              << "Copyright © 2024 Armin Sobhani\n"
              << "Display size: " << dm.w << "x" << dm.h << "\n"
              << "Window size:  " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT
              << std::endl;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, renderer, -1, 1);
    // Cleanup would go here
    // but emscripten_set_main_loop never returns
    // so it's omitted
#else
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {   while (SDL_PollEvent(&e) != 0)
            if (e.type == SDL_QUIT)
                quit = true;
        main_loop(renderer);
    }

    // free memory
    for (int i = 0; i < ROWS; ++i)
    {   delete[] grid[i];
        delete[] new_grid[i];
    }
    delete[] grid;
    delete[] new_grid;

    // destroy window and renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif // __EMSCRIPTEN__
}

