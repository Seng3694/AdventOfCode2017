#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

#define LAYERS_MAX 100

static void parse_line(char *line, size_t length, void *userData) {
  uint8_t *const ranges = userData;
  const uint8_t id = strtol(line, &line, 10);
  ranges[id] = strtol(line + 2, NULL, 10);
}

typedef enum {
  DIRECTION_DOWN = 1,
  DIRECTION_UP = -1,
} direction;

typedef struct {
  int8_t position;
  int8_t direction;
} scanner;

static void move_scanner(scanner *const s, const uint8_t range) {
  const int8_t newPos = s->position + s->direction;
  if (newPos < 0) {
    s->position = 1;
    s->direction = DIRECTION_DOWN;
  } else if (newPos == range) {
    s->position = range - 2;
    s->direction = DIRECTION_UP;
  } else {
    s->position = newPos;
  }
}

static void tick(const uint8_t ranges[const LAYERS_MAX],
                 scanner scanners[const LAYERS_MAX]) {
  for (uint8_t i = 0; i < LAYERS_MAX; ++i) {
    if (ranges[i] > 0) {
      move_scanner(&scanners[i], ranges[i]);
    }
  }
}

static uint32_t solve_part1(const uint8_t ranges[const LAYERS_MAX]) {
  scanner scanners[LAYERS_MAX] = {0};
  for (uint8_t i = 0; i < LAYERS_MAX; ++i)
    scanners[i].direction = DIRECTION_DOWN;

  uint32_t severity = 0;
  for (uint8_t position = 0; position < LAYERS_MAX; ++position) {
    if (ranges[position] > 0 && scanners[position].position == 0)
      severity += (position * ranges[position]);
    tick(ranges, scanners);
  }

  return severity;
}

int main(void) {
  uint8_t ranges[LAYERS_MAX] = {0};
  AocReadFileLineByLine("day13/input.txt", parse_line, ranges);

  const uint32_t part1 = solve_part1(ranges);

  printf("%u\n", part1);
}
