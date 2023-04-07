#include <aoc/aoc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int32_t x;
  int32_t y;
} position;

static inline int32_t taxicab_distance(const position from, const position to) {
  return (int32_t)abs(from.x - to.x) + (int32_t)abs(from.y - to.y);
}

static int32_t solve_part1(const int32_t destination) {
  position pos = {0};
  int32_t current = 0;
  int32_t inc = 1;
  while (current + (4 * inc + 2) < destination) {
    current += 4 * inc + 2;
    pos.x -= 1;
    pos.y += 1;
    inc += 2;
  }

  // do one more rotation until destination is found
  // go right
  if (current + inc > destination) {
    pos.x += destination - current;
    goto taxi_cab;
  } else {
    current += inc;
    pos.x += inc;
  }
  // go up
  if (current + inc > destination) {
    pos.y -= destination - current;
    goto taxi_cab;
  } else {
    current += inc;
    pos.y -= inc;
  }

  inc++;
  // go left
  if (current + inc > destination) {
    pos.x -= destination - current;
    goto taxi_cab;
  } else {
    current += inc;
    pos.x -= inc;
  }
  // go down
  if (current + inc > destination) {
    pos.y += destination - current;
  }

taxi_cab:
  return taxicab_distance((position){0, 0}, pos) - 1;
}

int main(void) {
  const int32_t input = 361527;

  const int32_t part1 = solve_part1(input);
  printf("%d\n", part1);
}
