#include "game.h"

#include "config.h"

#include "stb_sprintf.h"

static int RenderCircle(SDL_Renderer* renderer, SDL_Point center_point, int radius, SDL_Color color);

static float to_radians(float degrees)
{
    return degrees * (3.141592f / 180.0f);
}

bool game_init(struct game_state* game_state, pcg32_random_t* rng, SDL_Renderer* renderer, bool reset)
{
    if (rng != NULL) {
        game_state->rng = rng;
    }

    game_state->paddles[0].x = 10;
    game_state->paddles[0].y = SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT / 2);
    game_state->paddles[0].w = PADDLE_WIDTH;
    game_state->paddles[0].h = PADDLE_HEIGHT;

    game_state->paddles[1].x = SCREEN_WIDTH - PADDLE_WIDTH - 10;
    game_state->paddles[1].y = SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT) / 2;
    game_state->paddles[1].w = PADDLE_WIDTH;
    game_state->paddles[1].h = PADDLE_HEIGHT;

    game_state->ball.position.x = SCREEN_WIDTH / 2;
    game_state->ball.position.y = pcg32_random_ranged(game_state->rng, BALL_RADIUS, SCREEN_HEIGHT - BALL_RADIUS);

    game_state->ball.color.r = 255;
    game_state->ball.color.g = 255;
    game_state->ball.color.b = 255;
    game_state->ball.color.a = SDL_ALPHA_OPAQUE;

    const uint32_t dir = pcg32_random_ranged(game_state->rng, 0, 3);

    if (dir == 0) {
        game_state->ball.velocity.x = SDL_sinf(to_radians(45.0f));
        game_state->ball.velocity.y = SDL_cosf(to_radians(45.0f));
    } else if (dir == 1) {
        game_state->ball.velocity.x = SDL_sinf(to_radians(135.0f));
        game_state->ball.velocity.y = SDL_cosf(to_radians(135.0f));
    } else if (dir == 2) {
        game_state->ball.velocity.x = SDL_sinf(to_radians(225.0f));
        game_state->ball.velocity.y = SDL_cosf(to_radians(225.0f));
    } else {
        game_state->ball.velocity.x = SDL_sinf(to_radians(315.0f));
        game_state->ball.velocity.y = SDL_cosf(to_radians(315.0f));
    }

    const uint32_t speed = pcg32_random_ranged(game_state->rng, 3, 6);
    game_state->ball.velocity.x *= speed;
    game_state->ball.velocity.y *= speed;

    if (!reset) {
        game_state->scoreboard.player_one = 0;
        game_state->scoreboard.player_two = 0;
    }

    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init(): %s", TTF_GetError());
        return false;
    }

    game_state->font = TTF_OpenFont("/usr/share/fonts/TTF/Roboto-Regular.ttf", 24);
    if (game_state->font == NULL) {
        SDL_Log("TTF_OpenFont(): %s", TTF_GetError());
        TTF_Quit();
        return false;
    }

    const SDL_Color white_color = {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

    char buffer[8] = {0};

    stbsp_snprintf(buffer, COUNT_OF(buffer), "%03d", game_state->scoreboard.player_one);
    game_state->player_score_surfaces[0] = TTF_RenderText_Solid(game_state->font, buffer, white_color);

    stbsp_snprintf(buffer, COUNT_OF(buffer), "%03d", game_state->scoreboard.player_two);
    game_state->player_score_surfaces[1] = TTF_RenderText_Solid(game_state->font, buffer, white_color);

    game_state->player_score_textures[0] = SDL_CreateTextureFromSurface(renderer, game_state->player_score_surfaces[0]);
    game_state->player_score_textures[1] = SDL_CreateTextureFromSurface(renderer, game_state->player_score_surfaces[1]);

    game_state->player_score_rects[0].x = SCREEN_WIDTH / 2 - game_state->player_score_surfaces[0]->w - 16;
    game_state->player_score_rects[0].y = 0;
    game_state->player_score_rects[0].w = game_state->player_score_surfaces[0]->w;
    game_state->player_score_rects[0].h = game_state->player_score_surfaces[0]->h;

    game_state->player_score_rects[1].x = SCREEN_WIDTH / 2 + game_state->player_score_surfaces[1]->w - 16;
    game_state->player_score_rects[1].y = 0;
    game_state->player_score_rects[1].w = game_state->player_score_surfaces[1]->w;
    game_state->player_score_rects[1].h = game_state->player_score_surfaces[1]->h;

    return true;
}

void game_free(struct game_state* game_state)
{
    SDL_FreeSurface(game_state->player_score_surfaces[0]);
    SDL_FreeSurface(game_state->player_score_surfaces[1]);
    SDL_DestroyTexture(game_state->player_score_textures[0]);
    SDL_DestroyTexture(game_state->player_score_textures[1]);
    TTF_Quit();
}

bool game_update(struct game_state* game_state, SDL_Renderer* renderer)
{
    const uint8_t* kb_state = SDL_GetKeyboardState(NULL);
    if (kb_state[SDL_SCANCODE_W]) {
        game_state->paddles[0].y -= 16;
        if (game_state->paddles[0].y < 10) {
            game_state->paddles[0].y = 10;
        }
    } else if (kb_state[SDL_SCANCODE_S]) {
        game_state->paddles[0].y += 16;
        if (game_state->paddles[0].y > SCREEN_HEIGHT-PADDLE_HEIGHT - 10) {
            game_state->paddles[0].y = SCREEN_HEIGHT-PADDLE_HEIGHT - 10;
        }
    }

    if (kb_state[SDL_SCANCODE_UP]) {
        game_state->paddles[1].y -= 16;
        if (game_state->paddles[1].y < 10) {
            game_state->paddles[1].y = 10;
        }
    } else if (kb_state[SDL_SCANCODE_DOWN]) {
        game_state->paddles[1].y += 16;
        if (game_state->paddles[1].y > SCREEN_HEIGHT-PADDLE_HEIGHT - 10) {
            game_state->paddles[1].y = SCREEN_HEIGHT-PADDLE_HEIGHT - 10;
        }
    }

    game_state->ball.position.x += game_state->ball.velocity.x;
    game_state->ball.position.y += game_state->ball.velocity.y;

    if (game_state->ball.position.x < PADDLE_WIDTH + BALL_RADIUS) {
        // Player 2 scores
        game_state->scoreboard.player_two += 1;
        game_free(game_state);
        game_init(game_state, NULL, renderer, true);
    }
    if (game_state->ball.position.x > SCREEN_WIDTH - PADDLE_WIDTH + BALL_RADIUS) {
        // Player 1 scores
        game_state->scoreboard.player_one += 1;
        game_free(game_state);
        game_init(game_state, NULL, renderer, true);
    } 

    if (game_state->ball.position.y > SCREEN_HEIGHT - BALL_RADIUS) {
        game_state->ball.position.y = SCREEN_HEIGHT - BALL_RADIUS;
        game_state->ball.velocity.y *= -1;
    }
    else if (game_state->ball.position.y < BALL_RADIUS) {
        game_state->ball.position.y = BALL_RADIUS;
        game_state->ball.velocity.y *= -1;
    }

    const SDL_Rect ballRect = {
        .x = game_state->ball.position.x - BALL_RADIUS,
        .y = game_state->ball.position.y - BALL_RADIUS,
        .w = BALL_RADIUS * 2,
        .h = BALL_RADIUS * 2
    };

    if (SDL_HasIntersection(&game_state->paddles[0], &ballRect)) {
        game_state->ball.velocity.x *= -1;
        game_state->ball.position.x += 1;
    }
    else if (SDL_HasIntersection(&game_state->paddles[1], &ballRect)) {
        game_state->ball.velocity.x *= -1;
        game_state->ball.position.x -= 1;
    }

    return true;
}

bool game_render(SDL_Renderer* renderer, struct game_state* game_state)
{
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) {
        SDL_Log("SDL_SetRenderDrawColor(): %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderClear(renderer) != 0) {
        SDL_Log("SDL_RenderClear(): %s", SDL_GetError());
        return false;
    }

    if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0) {
        SDL_Log("SDL_SetRenderDrawColor(): %s", SDL_GetError());
        return false;
    }
    
    if (SDL_RenderFillRects(renderer, game_state->paddles, COUNT_OF(game_state->paddles)) != 0) {
        SDL_Log("SDL_RenderDrawRect(): %s", SDL_GetError());
        return false;
    }

    if (RenderCircle(renderer, game_state->ball.position, BALL_RADIUS, game_state->ball.color) != 0) {
        SDL_Log("RenderCircle(): %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderCopy(renderer, game_state->player_score_textures[0], NULL, &game_state->player_score_rects[0]) != 0) {
        SDL_Log("SDL_RenderCopy(): %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderCopy(renderer, game_state->player_score_textures[1], NULL, &game_state->player_score_rects[1]) != 0) {
        SDL_Log("SDL_RenderCopy(): %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT) != 0) {
        SDL_Log("SDL_RenderDrawLine(): %s", SDL_GetError());
        return false;
    }

    SDL_RenderPresent(renderer);

    return true;
}

#define CIRCLE_VERTEX_COUNT 16

static int RenderCircle(SDL_Renderer* renderer, SDL_Point center_point, int radius, SDL_Color color)
{
    SDL_Vertex vertices[CIRCLE_VERTEX_COUNT + 1];

    int indices[CIRCLE_VERTEX_COUNT * 3 + 1];
    size_t num_indices = 0;

    const float segment_rotation_angle = (360.0f / CIRCLE_VERTEX_COUNT) * (3.141592f / 180.0f);

    vertices[0].position.x = center_point.x;
    vertices[0].position.y = center_point.y;

    vertices[0].color.r = color.r;
    vertices[0].color.g = color.g;
    vertices[0].color.b = color.b;
    vertices[0].color.a = color.a;

    float start_x = 0.0f - radius;
    float start_y = 0.0f;

    for (uint32_t i = 1; i < CIRCLE_VERTEX_COUNT + 1; ++i)
    {
        float rotation_angle = (i * segment_rotation_angle);

        vertices[i].position.x = SDL_cosf(rotation_angle) * start_x - SDL_sinf(rotation_angle) * start_y;
        vertices[i].position.y = SDL_cosf(rotation_angle) * start_y + SDL_sinf(rotation_angle) * start_x;

        vertices[i].position.x += center_point.x;
        vertices[i].position.y += center_point.y;

        vertices[i].color.r = color.r;
        vertices[i].color.g = color.g;
        vertices[i].color.b = color.b;
        vertices[i].color.a = color.a;

        indices[num_indices++] = 0;
        indices[num_indices++] = i;

        int index = (i + 1) % CIRCLE_VERTEX_COUNT;
        if (index == 0) {
            index = CIRCLE_VERTEX_COUNT;
        }
        indices[num_indices++] = index;
    }

    return SDL_RenderGeometry(renderer, NULL, vertices, CIRCLE_VERTEX_COUNT + 1, indices, num_indices);
}