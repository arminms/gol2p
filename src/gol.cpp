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
#include <chrono>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#else
#   include <cmrc/cmrc.hpp>
    CMRC_DECLARE(gol2p);
#endif // __EMSCRIPTEN__

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 3;
const int ROWS = WINDOW_HEIGHT / CELL_SIZE;
const int COLS = WINDOW_WIDTH / CELL_SIZE;

struct cellular_automaton_grid
{   cellular_automaton_grid(size_t rows, size_t cols)
    : rows_(rows), cols_(cols)
    {   std::random_device r;
        std::default_random_engine e(r());
        std::uniform_int_distribution<int> uniform_dist(0, 1);
        grid_ = new uint8_t*[rows_];
        new_grid_ = new uint8_t*[rows_];
        for (size_t i = 0; i < rows_; ++i)
        {   grid_[i] = new uint8_t[cols_];
            new_grid_[i] = new uint8_t[cols_];
            for (size_t j = 0; j < cols_; ++j)
                grid_[i][j] = uniform_dist(e);
        }
    }

    ~cellular_automaton_grid()
    {   for (size_t i = 0; i < rows_; ++i)
        {   delete[] grid_[i];
            delete[] new_grid_[i];
        }
        delete[] grid_;
        delete[] new_grid_;
    }

    size_t count_live_neighbors(size_t x, size_t y)
    {   size_t live_neighbors = 0;
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
            {   if (i == 0 && j == 0) continue;
                size_t new_x = (x + i + rows_) % rows_;
                size_t new_y = (y + j + cols_) % cols_;
                live_neighbors += grid_[new_x][new_y];
            }
        return live_neighbors;
    }

    void update()
    {   for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
            {   size_t live_neighbors = count_live_neighbors(i, j);
                if
                (   grid_[i][j] == 1
                && (live_neighbors < 2 || live_neighbors > 3)
                )
                    new_grid_[i][j] = 0;
                else if (grid_[i][j] == 0 && live_neighbors == 3)
                    new_grid_[i][j] = 1;
                else
                    new_grid_[i][j] = grid_[i][j];
            }
        for (size_t i = 0; i < rows_; ++i)
            for (size_t j = 0; j < cols_; ++j)
                grid_[i][j] = new_grid_[i][j];
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    uint8_t** grid() const { return grid_; }

private:
    size_t        rows_;
    size_t        cols_;
    uint8_t**     grid_;
    uint8_t** new_grid_;
};


struct rendering_context
{   rendering_context(size_t width, size_t height, size_t cell_size)
    :   width_(width)
    ,   height_(height)
    ,   cell_size_(cell_size)
    ,   frame_count_(0)
    ,   fps_(0)
    ,   grid_(nullptr)
    {   SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        window_ = SDL_CreateWindow
        (   "John Conway's Game of Life"
        ,   SDL_WINDOWPOS_UNDEFINED
        ,   SDL_WINDOWPOS_UNDEFINED
        ,   width_
        ,   height_
        ,   0
        );
        renderer_ = SDL_CreateRenderer
        (   window_
        ,   -1
        ,   SDL_RENDERER_ACCELERATED
        );
#ifdef __EMSCRIPTEN__
        font_ = TTF_OpenFont("src/font.ttf", 32);
#else
        auto fs = cmrc::gol2p::get_filesystem();
        auto font_data = fs.open("font.ttf");
        font_ = TTF_OpenFontRW
        (   SDL_RWFromConstMem(font_data.begin(), font_data.size())
        ,   1
        ,   32
        );
#endif // __EMSCRIPTEN__
        if (font_ == nullptr)
        {   std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            SDL_DestroyRenderer(renderer_);
            SDL_DestroyWindow(window_);
            TTF_Quit();
            SDL_Quit();
            exit(1);
        }
        grid_ = new cellular_automaton_grid
        (   height_ / cell_size_
        ,   width_  / cell_size_
        );
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    ~rendering_context()
    {   if (grid_) delete grid_;
        TTF_CloseFont(font_);
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        TTF_Quit();
        SDL_Quit();
    }

    void render_grid()
    {   for (size_t i = 0; i < grid_->rows(); ++i)
            for (size_t j = 0; j < grid_->cols(); ++j)
            {   SDL_Rect cell;
                cell.x = j * cell_size_;
                cell.y = i * cell_size_;
                cell.w = cell_size_;
                cell.h = cell_size_;
                SDL_SetRenderDrawColor
                (   renderer_
                ,   grid_->grid()[i][j] * 0
                ,   grid_->grid()[i][j] * 255
                ,   grid_->grid()[i][j] * 0
                ,   255
                );
                SDL_RenderFillRect(renderer_, &cell);
            }
    }

    void render_fps()
    {   SDL_Color color = {255, 255, 0, 255}; // Yellow
        std::string fps_text = "FPS: " + (0 == fps_ ? "" : std::to_string(fps_));
        SDL_Surface* sur = TTF_RenderText_Blended(font_, fps_text.c_str(), color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer_, sur);
        SDL_Rect dstrect = {10, 10, sur->w, sur->h};
        SDL_FreeSurface(sur);
        SDL_RenderCopy(renderer_, tex, NULL, &dstrect);
        SDL_DestroyTexture(tex);
    }

    void render()
    {   frame_count_++;
        grid_->update();
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
        render_grid();
        render_fps();
        SDL_RenderPresent(renderer_);
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time_;
        if (elapsed.count() >= 1.0)
        {   fps_ = frame_count_;
            frame_count_ = 0;
            start_time_ = end_time;
        }
    }

private:
    size_t                  width_;
    size_t                 height_;
    size_t              cell_size_;
    SDL_Window*            window_;
    SDL_Renderer*        renderer_;
    TTF_Font*                font_;
    size_t            frame_count_;
    size_t                    fps_;
    cellular_automaton_grid* grid_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
};

void main_loop(void* rc_)
{   rendering_context *rc = (rendering_context*)rc_;
    rc->render();
}

int main(int argc, char* args[])
{   SDL_SetMainReady();
    rendering_context rc(WINDOW_WIDTH, WINDOW_HEIGHT, CELL_SIZE);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    std::cout << "John Conway's Game of Life v1.1\n"
              << "Copyright © 2024 Armin Sobhani\n"
              << "Display size: " << dm.w << "x" << dm.h << "\n"
              << "Window size:  " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT
              << std::endl;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, &rc, -1, 1);
#else
    bool quit = false;
    SDL_Event e;
    while (!quit)
    {   while (SDL_PollEvent(&e) != 0)
            if (e.type == SDL_QUIT)
                quit = true;
        main_loop(&rc);
    }
#endif // __EMSCRIPTEN__
}

