#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) (((x) > 0) - ((x) < 0))

typedef struct {
  int32_t x;
  int32_t y;
} position;

typedef enum {
  DIR_N,
  DIR_NW,
  DIR_NE,
  DIR_S,
  DIR_SW,
  DIR_SE,
} direction;

static const position MOVE_VALUES[] = {
    {0, 1},  // N
    {-1, 1}, // NW
    {1, 0},  // NE
    {0, -1}, // S
    {-1, 0}, // SW
    {1, -1}, // SE
};

#define AOC_T uint8_t
#define AOC_T_NAME Dirs
#include <aoc/array.h>

static void parse(const char *input, AocArrayDirs *const dirs) {
  while (*input) {
    direction dir = *input == 's' ? DIR_S : DIR_N;
    input++;
    if (*input == 'w') {
      dir += 1;
      input++;
    } else if (*input == 'e') {
      dir += 2;
      input++;
    }
    input++; // ,
    AocArrayDirsPush(dirs, dir);
  }
}

static uint32_t distance(const position pos) {
  if (SIGN(pos.x) == SIGN(pos.y))
    return abs(pos.x + pos.y);
  else
    return MAX(abs(pos.x), abs(pos.y));
}

static void solve(const AocArrayDirs *const dirs, uint32_t *const part1,
                  uint32_t *const part2) {
  uint32_t furthest = 0;
  position current = {0};
  for (size_t i = 0; i < dirs->length; ++i) {
    const position move = MOVE_VALUES[dirs->items[i]];
    current.x += move.x;
    current.y += move.y;
    const uint32_t d = distance(current);
    if (d > furthest)
      furthest = d;
  }
  *part1 = distance(current);
  *part2 = furthest;
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day11/input.txt", &input, &length);
  AocArrayDirs dirs = {0};
  AocArrayDirsCreate(&dirs, 8300);
  parse(input, &dirs);

  uint32_t part1 = 0;
  uint32_t part2 = 0;
  solve(&dirs, &part1, &part2);

  printf("%u\n", part1);
  printf("%u\n", part2);

  AocArrayDirsDestroy(&dirs);
  free(input);
}
