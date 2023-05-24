#pragma once

/* Stolen from https://stackoverflow.com/a/4415646 */
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define PADDLE_WIDTH 32
#define PADDLE_HEIGHT 128

#define BALL_RADIUS 8