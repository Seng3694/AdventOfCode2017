#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int8_t scannerCache[64];
  int8_t cycleLength;
  int8_t depth;
  int8_t range;
} layer;

typedef struct {
  layer layers[64];
  uint8_t count;
} context;

static inline int8_t calc_cycle_length(const int8_t range) {
  return (range - 1) * 2;
}

static void parse_line(char *line, size_t length, void *userData) {
  context *const ctx = userData;
  layer *const current = &ctx->layers[ctx->count];
  current->depth = strtol(line, &line, 10);
  current->range = strtol(line + 2, NULL, 10);
  current->cycleLength = calc_cycle_length(current->range);
  for (int8_t i = 0; i < current->cycleLength; ++i)
    current->scannerCache[i] = (i + current->depth) % current->cycleLength;
  ctx->count++;
}

static uint32_t solve_part1(const context *const ctx) {
  uint32_t severity = 0;
  for (uint8_t i = 0; i < ctx->count; ++i) {
    const layer *const current = &ctx->layers[i];
    if (current->scannerCache[0] == 0)
      severity += current->depth * current->range;
  }
  return severity;
}

static uint32_t solve_part2(const context *const ctx) {
  for (uint32_t delay = 1;; ++delay) {
    for (uint8_t i = 0; i < ctx->count; ++i) {
      const layer *const current = &ctx->layers[i];
      if (current->scannerCache[delay % current->cycleLength] == 0)
        goto next;
    }
    return delay;
  next:;
  }
}

int main(void) {
  context ctx = {0};
  AocReadFileLineByLine("day13/input.txt", parse_line, &ctx);

  const uint32_t part1 = solve_part1(&ctx);
  const uint32_t part2 = solve_part2(&ctx);

  printf("%u\n", part1);
  printf("%u\n", part2);
}
