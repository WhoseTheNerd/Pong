#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "random.h"

struct pong_ball
{
    SDL_Point position;
    SDL_FPoint velocity;
    SDL_Color color;
};

struct scoreboard
{
    uint32_t player_one;
    uint32_t player_two;
};

struct game_state
{
    SDL_Rect paddles[2];
    struct pong_ball ball;

    struct scoreboard scoreboard;

    pcg32_random_t* rng;
    TTF_Font* font;
    SDL_Surface* player_score_surfaces[2];
    SDL_Texture* player_score_textures[2];
    SDL_Rect player_score_rects[2];
};

bool game_init(struct game_state* game_state, pcg32_random_t* rng, SDL_Renderer* renderer, bool reset);
void game_free(struct game_state* game_state);

bool game_update(struct game_state* game_state, SDL_Renderer* renderer);
bool game_render(SDL_Renderer* renderer, struct game_state* game_state);
