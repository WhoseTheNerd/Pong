#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>

#include <SDL2/SDL.h>

#include "random.h"

/* Stolen from https://stackoverflow.com/a/4415646 */
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define PADDLE_WIDTH 32
#define PADDLE_HEIGHT 128

#define BALL_RADIUS 8

static int RenderCircle(SDL_Renderer* renderer, SDL_Point center_point, int radius, SDL_Color color);

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    pcg32_random_t rng = PCG32_INITIALIZER;

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

    SDL_Rect paddles[2] = {0};
    paddles[0].x = 10;
    paddles[0].y = SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT / 2);
    paddles[0].w = PADDLE_WIDTH;
    paddles[0].h = PADDLE_HEIGHT;

    paddles[1].x = SCREEN_WIDTH - PADDLE_WIDTH - 10;
    paddles[1].y = SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT) / 2;
    paddles[1].w = PADDLE_WIDTH;
    paddles[1].h = PADDLE_HEIGHT;

    SDL_Point ball_position = { .x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2 };

    const SDL_Color ball_color = {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE};

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

        const uint8_t* kb_state = SDL_GetKeyboardState(NULL);

        if (kb_state[SDL_SCANCODE_W]) {
            paddles[0].y -= 16;
            if (paddles[0].y < 10) {
                paddles[0].y = 10;
            }
        }
        else if (kb_state[SDL_SCANCODE_S]) {
            paddles[0].y += 16;
            if (paddles[0].y > SCREEN_HEIGHT-PADDLE_HEIGHT - 10) {
                paddles[0].y = SCREEN_HEIGHT-PADDLE_HEIGHT - 10;
            }
        }

        if (kb_state[SDL_SCANCODE_UP]) {
            paddles[1].y -= 16;
            if (paddles[1].y < 10) {
                paddles[1].y = 10;
            }
        }
        else if (kb_state[SDL_SCANCODE_DOWN]) {
            paddles[1].y += 16;
            if (paddles[1].y > SCREEN_HEIGHT-PADDLE_HEIGHT - 10) {
                paddles[1].y = SCREEN_HEIGHT-PADDLE_HEIGHT - 10;
            }
        }

        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) {
            SDL_Log("SDL_SetRenderDrawColor(): %s", SDL_GetError());
            goto cleanup;
        }

        if (SDL_RenderClear(renderer) != 0) {
            SDL_Log("SDL_RenderClear(): %s", SDL_GetError());
            goto cleanup;
        }

        if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0) {
            SDL_Log("SDL_SetRenderDrawColor(): %s", SDL_GetError());
            goto cleanup;
        }
        
        if (SDL_RenderFillRects(renderer, paddles, COUNT_OF(paddles)) != 0) {
            SDL_Log("SDL_RenderDrawRect(): %s", SDL_GetError());
            goto cleanup;
        }

        if (RenderCircle(renderer, ball_position, BALL_RADIUS, ball_color) != 0) {
            SDL_Log("RenderCircle(): %s", SDL_GetError());
            goto cleanup;
        }

        SDL_RenderPresent(renderer);
    }

cleanup:
    SDL_DestroyRenderer(renderer);
cleanup_2:
    SDL_DestroyWindow(window);
cleanup_1:
    SDL_Quit();

    return EXIT_SUCCESS;
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

        vertices[i].position.x = cosf(rotation_angle) * start_x - sinf(rotation_angle) * start_y;
        vertices[i].position.y = cosf(rotation_angle) * start_y + sinf(rotation_angle) * start_x;

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