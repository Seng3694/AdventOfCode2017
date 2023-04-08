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

const position DIRECTIONS[] = {
    {1, 0},  // right
    {0, -1}, // top
    {-1, 0}, // left
    {0, 1},  // down
};

#define POS_L ((position){-1, 0})
#define POS_T ((position){0, -1})
#define POS_R ((position){1, 0})
#define POS_B ((position){0, 1})
#define POS_TL ((position){-1, -1})
#define POS_TR ((position){1, -1})
#define POS_BL ((position){-1, 1})
#define POS_BR ((position){1, 1})

const position CHECKS[4][4] = {
    {POS_L, POS_TL, POS_T, POS_TR}, // going right
    {POS_B, POS_BL, POS_L, POS_TL}, // going up
    {POS_R, POS_BR, POS_B, POS_BL}, // going left
    {POS_T, POS_TR, POS_R, POS_BR}, // going down,
};

const position EMPTY_POS = {INT32_MAX, INT32_MAX};

static inline uint32_t calc_position_hash(const position *const p) {
  return 54812489 * ((uint32_t)(p->x) ^ 95723417) *
         ((uint32_t)(p->y) ^ 74297323);
}

static inline bool position_equals(const position *const a,
                                   const position *const b) {
  return a->x == b->x && a->y == b->y;
}

#define AOC_KEY_T position
#define AOC_KEY_T_NAME Pos
#define AOC_VALUE_T int32_t
#define AOC_VALUE_T_NAME I32
#define AOC_KEY_T_EMPTY EMPTY_POS
#define AOC_KEY_T_HFUNC calc_position_hash
#define AOC_KEY_T_EQUALS position_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

static int32_t solve_part2(const int32_t input) {
  AocHashmapPosI32 positions = {0};
  AocHashmapPosI32Create(&positions, 64);

  int32_t inc = 1;
  int32_t solution = 0;
  position current = {0, 0};
  AocHashmapPosI32Insert(&positions, current, 1);

  for (;;) {
    for (int32_t d = 0; d < 4; ++d) {
      for (int32_t i = 0; i < inc; ++i) {
        current.x += DIRECTIONS[d].x;
        current.y += DIRECTIONS[d].y;
        int32_t sum = 0;
        for (size_t c = 0; c < 4; ++c) {
          int32_t value = 0;
          const position p = {
              current.x + CHECKS[d][c].x,
              current.y + CHECKS[d][c].y,
          };
          if (AocHashmapPosI32Get(&positions, p, &value)) {
            sum += value;
          }
        }
        AocHashmapPosI32Insert(&positions, current, sum);
        if (sum > input) {
          solution = sum;
          goto finish;
        }
      }
      if (d % 2 != 0) // after every 2 turns increase the amount
        inc++;
    }
  }

finish:
  AocHashmapPosI32Destroy(&positions);
  return solution;
}

int main(void) {
  const int32_t input = 361527;

  const int32_t part1 = solve_part1(input);
  const int32_t part2 = solve_part2(input);

  printf("%d\n", part1);
  printf("%d\n", part2);
}
