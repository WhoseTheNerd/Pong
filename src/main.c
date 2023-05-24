#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "random.h"
#include "game.h"
#include "config.h"

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    pcg32_random_t rng = PCG32_INITIALIZER;
    pcg32_srandom_r(&rng, clock(), clock());

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init(): %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        SDL_Log("SDL_CreateWindow(): %s", SDL_GetError());
        goto cleanup_1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_Log("SDL_CreateRenderer(): %s", SDL_GetError());
        goto cleanup_2;
    }

    struct game_state game_state = {0};

    if (!game_init(&game_state, &rng)) {
        SDL_Log("Failed to initialize game state!");
        goto cleanup;
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

        if (!game_update(&game_state)) {
            SDL_Log("Failed to update the game state!");
            goto cleanup;
        }

        if (!game_render(renderer, &game_state)) {
            SDL_Log("Failed to render the game state!");
            goto cleanup;
        }
        
    }

cleanup:
    SDL_DestroyRenderer(renderer);
cleanup_2:
    SDL_DestroyWindow(window);
cleanup_1:
    SDL_Quit();

    return EXIT_SUCCESS;
}