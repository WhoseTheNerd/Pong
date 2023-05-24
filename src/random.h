#pragma once

#include <inttypes.h>

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

#define PCG32_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq);
uint32_t pcg32_random_r(pcg32_random_t* rng);
uint32_t pcg32_random_ranged(pcg32_random_t* rng, uint32_t min, uint32_t max);