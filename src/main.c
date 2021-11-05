#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

static void error(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    vfprintf(stderr, format, list);
    va_end(list);
    abort();
}

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    SDL_Window* window = {0};

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        error("SDL failed to initialize: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, 0);
    if (window == NULL) {
        error("Failed to create SDL window: %s\n", SDL_GetError());
    }

    bool should_continue = true;
    while (should_continue)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_QUIT:
                should_continue = false;
                break;
            default: break;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}