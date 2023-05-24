#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "random.h"

struct pong_ball
{
    SDL_Point position;
    SDL_FPoint velocity;
    SDL_Color color;
};

struct game_state
{
    SDL_Rect paddles[2];
    struct pong_ball ball;

    pcg32_random_t* rng;
};

bool game_init(struct game_state* game_state, pcg32_random_t* rng);

bool game_update(struct game_state* game_state);
bool game_render(SDL_Renderer* renderer, struct game_state* game_state);
