#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int32_t x;
  int32_t y;
} position;

const position EMPTY_POS = {UINT32_MAX, UINT32_MAX};

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
#define AOC_VALUE_T char
#define AOC_VALUE_T_NAME Char
#define AOC_KEY_T_EMPTY EMPTY_POS
#define AOC_KEY_T_HFUNC calc_position_hash
#define AOC_KEY_T_EQUALS position_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

typedef struct {
  AocHashmapPosChar map;
  position start;
} context;

static void parse(char *line, size_t length, void *userData,
                  const size_t lineNumber) {
  context *const ctx = userData;
  position pos = {.x = 0, .y = (int32_t)lineNumber};

  if (lineNumber == 0) {
    for (; *line; line++, pos.x++) {
      if (*line != ' ') {
        ctx->start.x = pos.x;
        ctx->start.y = 0;
        AocHashmapPosCharInsert(&ctx->map, pos, *line);
        return;
      }
    }
  } else {
    for (; *line; line++, pos.x++)
      if (*line != ' ') {
        AocHashmapPosCharInsert(&ctx->map, pos, *line);
      }
  }
}

typedef enum {
  DIRECTION_UP,
  DIRECTION_RIGHT,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
} direction;

static const position moves[] = {
    [DIRECTION_UP] = {0, -1},
    [DIRECTION_RIGHT] = {1, 0},
    [DIRECTION_DOWN] = {0, 1},
    [DIRECTION_LEFT] = {-1, 0},
};

static inline position add_pos(const position p1, const position p2) {
  return (position){p1.x + p2.x, p1.y + p2.y};
}

static inline position move(const position p, const direction d) {
  return add_pos(p, moves[d]);
}

static void solve_both(const context *const ctx) {
  const AocHashmapPosChar *const map = &ctx->map;

  char buffer[16] = {0};
  size_t bufferLength = 0;

  position pos = ctx->start;
  direction dir = DIRECTION_DOWN;
  uint32_t steps = 1; // first map piece

  uint32_t hash = 0;
  char c;

  while (true) {
    position next = move(pos, dir);
    // check if there is a map piece at the next location
    if (AocHashmapPosCharContains(map, next, &hash)) {
      AocHashmapPosCharGetPrehashed(map, next, hash, &c);
      if (c >= 'A' && c <= 'Z')
        buffer[bufferLength++] = c;
    } else /* if not check other directions */ {
      switch (dir) {
      case DIRECTION_UP:   // moves up, check left and right
      case DIRECTION_DOWN: // moves down, check left and right
        next = move(pos, DIRECTION_LEFT);
        if (AocHashmapPosCharContains(map, next, &hash)) {
          dir = DIRECTION_LEFT;
          AocHashmapPosCharGetPrehashed(map, next, hash, &c);
          if (c >= 'A' && c <= 'Z')
            buffer[bufferLength++] = c;
        } else {
          next = move(pos, DIRECTION_RIGHT);
          if (AocHashmapPosCharContains(map, next, &hash)) {
            dir = DIRECTION_RIGHT;
            AocHashmapPosCharGetPrehashed(map, next, hash, &c);
            if (c >= 'A' && c <= 'Z')
              buffer[bufferLength++] = c;
          } else {
            // DEAD END
            goto finish;
          }
        }
        break;
      case DIRECTION_RIGHT: // moves right, check up and down
      case DIRECTION_LEFT:  // moves left, check up and down
        next = move(pos, DIRECTION_UP);
        if (AocHashmapPosCharContains(map, next, &hash)) {
          dir = DIRECTION_UP;
          AocHashmapPosCharGetPrehashed(map, next, hash, &c);
          if (c >= 'A' && c <= 'Z')
            buffer[bufferLength++] = c;
        } else {
          next = move(pos, DIRECTION_DOWN);
          if (AocHashmapPosCharContains(map, next, &hash)) {
            dir = DIRECTION_DOWN;
            AocHashmapPosCharGetPrehashed(map, next, hash, &c);
            if (c >= 'A' && c <= 'Z')
              buffer[bufferLength++] = c;
          } else {
            // DEAD END
            goto finish;
          }
        }
        break;
      }
    }

    pos = next;
    steps++;
  }

finish:
  printf("%s\n", buffer);
  printf("%u\n", steps);
}

int main(void) {
  context ctx = {0};
  AocHashmapPosCharCreate(&ctx.map, 32768);
  AocReadFileLineByLineEx("day19/input.txt", parse, &ctx);

  solve_both(&ctx);

  AocHashmapPosCharDestroy(&ctx.map);
}
