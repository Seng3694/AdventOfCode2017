#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

#define MAP_PART_SIZE 8

typedef struct {
  int64_t x;
  int64_t y;
} vector2i;

const vector2i EMPTY_VEC = {INT64_MIN, INT64_MIN};

static inline uint32_t calc_vec_hash(const vector2i *const vec) {
  return 54812489 * ((uint32_t)vec->x ^ 95723417) *
         ((uint32_t)vec->y ^ 74297323);
}

static inline bool vec_equals(const vector2i *const a,
                              const vector2i *const b) {
  return a->x == b->x && a->y == b->y;
}

#define AOC_KEY_T vector2i
#define AOC_KEY_T_NAME Vec
#define AOC_VALUE_T char
#define AOC_VALUE_T_NAME State
#define AOC_KEY_T_EMPTY EMPTY_VEC
#define AOC_KEY_T_HFUNC calc_vec_hash
#define AOC_KEY_T_EQUALS vec_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

// order matters for turn functions
typedef enum {
  DIRECTION_UP,
  DIRECTION_RIGHT,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
} direction;

typedef struct {
  vector2i position;
  direction dir;
} carrier;

typedef struct {
  AocHashmapVecState map;
  carrier carrier;
} context;

static inline direction turn_right(const direction dir) {
  return (direction)((((uint8_t)dir) + 1) & 3);
}

static inline direction turn_left(const direction dir) {
  return (direction)((((uint8_t)dir) - 1) & 3);
}

static const vector2i MOVE_TABLE[] = {
    [DIRECTION_UP] = {0, -1},
    [DIRECTION_RIGHT] = {1, 0},
    [DIRECTION_DOWN] = {0, 1},
    [DIRECTION_LEFT] = {-1, 0},
};

static inline void add_vector(vector2i *const vec, const vector2i other) {
  vec->x += other.x;
  vec->y += other.y;
}

static inline void move(context *const ctx) {
  add_vector(&ctx->carrier.position, MOVE_TABLE[ctx->carrier.dir]);
}

static void parse(const char *const path, context *const ctx) {
  char *content = NULL;
  size_t length = 0;
  AocReadFileToString(path, &content, &length);
  // map is 25x25 big
  // start is at 12/12
  AocHashmapVecStateCreate(&ctx->map, 4096);
  ctx->carrier = (carrier){
      .position = {12, 12},
      .dir = DIRECTION_UP,
  };
  for (int64_t y = 0; y < 25; ++y) {
    for (int64_t x = 0; x < 25; ++x) {
      AocHashmapVecStateInsert(&ctx->map, (vector2i){x, y},
                               content[y * 26 + x]);
    }
  }
  free(content);
}

static char get_state(context *const ctx) {
  char out = '.';
  uint32_t hash = 0;
  if (AocHashmapVecStateContains(&ctx->map, ctx->carrier.position, &hash))
    AocHashmapVecStateGet(&ctx->map, ctx->carrier.position, &out);
  return out;
}

static void set_state(context *const ctx, const char state) {
  uint32_t hash = 0;
  if (AocHashmapVecStateContains(&ctx->map, ctx->carrier.position, &hash))
    AocHashmapVecStateRemovePreHashed(&ctx->map, ctx->carrier.position, hash);

  AocHashmapVecStateInsertPreHashed(&ctx->map, ctx->carrier.position, state,
                                    hash);
}

static uint32_t solve_part1(context *const ctx) {
  uint32_t infections = 0;
  for (uint32_t i = 0; i < 10000; ++i) {
    const bool isInfected = get_state(ctx) == '#';
    ctx->carrier.dir =
        isInfected ? turn_right(ctx->carrier.dir) : turn_left(ctx->carrier.dir);

    if (!isInfected) {
      infections++;
      set_state(ctx, '#');
    } else {
      set_state(ctx, '.');
    }

    move(ctx);
  }
  return infections;
}

int main(void) {
  context ctx = {0};
  parse("day22/input.txt", &ctx);

  const uint32_t part1 = solve_part1(&ctx);

  printf("%u\n", part1);

  AocHashmapVecStateDestroy(&ctx.map);
}
