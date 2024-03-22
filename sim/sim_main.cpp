#include <SDL.h>

#include <memory>
#include <chrono>
#include <deque>
#include <fstream>
#include <sstream>

#include <verilated.h>
#include <iostream>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"

const int screen_width = 1024;
const int screen_height = 768;

// 640x480
const int vga_width = 800;
const int vga_height = 525;

double sc_time_stamp()
{
    return 0.0;
}

void advance_clk(const std::unique_ptr<Vtop>& top)
{
    static int clock_counter = 0;

    top->CLOCK_50 = clock_counter & 0x1;
    top->CLOCK_PIX = clock_counter & 0x2;
    top->contextp()->timeInc(1);
    top->eval();    
    clock_counter++;
}

int main(int argc, char **argv, char **env)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Huedeon GPU",
        SDL_WINDOWPOS_UNDEFINED_DISPLAY(1),
        SDL_WINDOWPOS_UNDEFINED,
        screen_width,
        screen_height,
        0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");


    const size_t pixels_size = vga_width * vga_height * 4;
    unsigned char *pixels = new unsigned char[pixels_size];

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, vga_width, vga_height);

    bool restart_model;
    do {

        // Construct a VerilatedContext to hold simulation time, etc.
        // Multiple modules (made later below with Vtop) may share the same
        // context to share time, or modules may have different contexts if
        // they should be independent from each other.

        // Using unique_ptr is similar to
        // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
        const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

        // Set debug level, 0 is off, 9 is highest presently used
        // May be overridden by commandArgs argument parsing
        //contextp->debug(0);

        // Randomization reset policy
        // May be overridden by commandArgs argument parsing
        contextp->randReset(0);

        // Verilator must compute traced signals
        contextp->traceEverOn(true);

        // Pass arguments so Verilated code can see them, e.g. $value$plusargs
        // This needs to be called before you create any model
        contextp->commandArgs(argc, argv);

        restart_model = false;

        // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
        // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
        // "TOP" will be the hierarchical name of the module.
        const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

        SDL_Event e;
        bool quit = false;

        auto tp_frame = std::chrono::high_resolution_clock::now();
        auto tp_clk = std::chrono::high_resolution_clock::now();
        auto tp_now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_clk;

        unsigned int frame_counter = 0;
        bool was_vsync = false;

        size_t pixel_index = 0;

        top->CLOCK_50 = 0;
        top->CLOCK_PIX = 0;
        top->KEY = 0b1111;

        while (!contextp->gotFinish() && !quit)
        {
            advance_clk(top);

            if (!top->CLOCK_50)
                continue;
                
            if (top->CLOCK_PIX) {
                // Update video display
                if (was_vsync && top->VGA_VS)
                {
                    pixel_index = 0;
                    was_vsync = false;
                }

                pixels[pixel_index] = top->VGA_R;
                pixels[pixel_index + 1] = top->VGA_G;
                pixels[pixel_index + 2] = top->VGA_B;
                pixels[pixel_index + 3] = 255;
                pixel_index = (pixel_index + 4) % (pixels_size);

                if (!top->VGA_VS && !was_vsync)
                {
                    was_vsync = true;
                    void *p;
                    int pitch;
                    SDL_LockTexture(texture, NULL, &p, &pitch);
                    assert(pitch == vga_width * 4);
                    memcpy(p, pixels, vga_width * vga_height * 4);
                    SDL_UnlockTexture(texture);
                }
            }

            tp_now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration_frame = tp_now - tp_frame;

            if (contextp->time() % 2000000 == 0)
            {
                duration_clk = tp_now - tp_clk;
                tp_clk = tp_now;
            }

            if (duration_frame.count() >= 1.0 / 60.0)
            {
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYUP)
                    {
                        switch (e.key.keysym.sym)
                        {
                            case SDLK_F1:
                                top->KEY = top->KEY | 0b0001;
                                break;
                            case SDLK_F2:
                                top->KEY = top->KEY | 0b0010;
                                break;
                            case SDLK_F3:
                                top->KEY = top->KEY | 0b0100;
                                break;
                            case SDLK_F4:
                                top->KEY = top->KEY | 0b1000;
                                break;
                        case SDLK_F12:
                            quit = true;
                            restart_model = true;
                            break;
                        default:
                            break;
                        }
                    }
                    else if (e.type == SDL_KEYDOWN)
                    {
                        if (e.key.repeat == 0)
                        {
                            switch (e.key.keysym.sym)
                            {
                            case SDLK_F1:
                                top->KEY = top->KEY & 0b1110;
                                break;
                            case SDLK_F2:
                                top->KEY = top->KEY & 0b1101;
                                break;
                            case SDLK_F3:
                                top->KEY = top->KEY & 0b1011;
                                break;
                            case SDLK_F4:
                                top->KEY = top->KEY & 0b0111;
                                break;
                            case SDLK_F12:
                                std::cout << "Reset context\n";
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }

                int draw_w, draw_h;
                SDL_GL_GetDrawableSize(window, &draw_w, &draw_h);

                int scale_x, scale_y;
                scale_x = draw_w / screen_width;
                scale_y = draw_h / screen_height;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                if (frame_counter % 100 == 0)
                {
                    std::cout << "Clk speed: " << 1.0 / (duration_clk.count()) << " MHz\n";
                }

                tp_frame = tp_now;
                frame_counter++;

                SDL_Rect vga_r = {0, scale_x * (screen_height - vga_height - 1), scale_x * vga_width, scale_y * vga_height};
                SDL_RenderCopy(renderer, texture, NULL, &vga_r);

                SDL_RenderPresent(renderer);
            }
        }

        // Final model cleanup
        top->final();
    } while (restart_model);

    SDL_DestroyTexture(texture);

    delete[] pixels;

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
