#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int64_t x;
  int64_t y;
} vector2i;

// order matters
typedef enum {
  STATE_CLEAN,
  STATE_WEAKENED,
  STATE_INFECTED,
  STATE_FLAGGED,
} state;

static inline state next_state(const state s) {
  return (state)((((uint8_t)s) + 1) & 3);
}

// goes from clean to infected and from infected to clean
static inline state toggle_state(const state s) {
  return (state)((((uint8_t)s) + 2) & 3);
}

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
#define AOC_VALUE_T state
#define AOC_VALUE_T_NAME State
#define AOC_KEY_T_EMPTY EMPTY_VEC
#define AOC_KEY_T_HFUNC calc_vec_hash
#define AOC_KEY_T_EQUALS vec_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

// order matters
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

static inline direction turn_back(const direction dir) {
  return (direction)((((uint8_t)dir) + 2) & 3);
}

static inline direction dont_turn(const direction dir) {
  return dir;
}

typedef direction (*turn_func)(const direction);

static const turn_func TURN_LOOKUP[] = {
    [STATE_CLEAN] = turn_left,
    [STATE_WEAKENED] = dont_turn,
    [STATE_INFECTED] = turn_right,
    [STATE_FLAGGED] = turn_back,
};

static const vector2i MOVE_LOOKUP[] = {
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
  add_vector(&ctx->carrier.position, MOVE_LOOKUP[ctx->carrier.dir]);
}

static void parse(const char *const content, context *const ctx) {
  // map is 25x25 big
  // start is at 12/12
  ctx->carrier = (carrier){
      .position = {12, 12},
      .dir = DIRECTION_UP,
  };
  for (int64_t y = 0; y < 25; ++y) {
    for (int64_t x = 0; x < 25; ++x) {
      AocHashmapVecStateInsert(&ctx->map, (vector2i){x, y},
                               content[y * 26 + x] == '#' ? STATE_INFECTED
                                                          : STATE_CLEAN);
    }
  }
}

static state get_state(context *const ctx) {
  state out = STATE_CLEAN;
  uint32_t hash = 0;
  if (AocHashmapVecStateContains(&ctx->map, ctx->carrier.position, &hash))
    AocHashmapVecStateGet(&ctx->map, ctx->carrier.position, &out);
  return out;
}

static void set_state(context *const ctx, const state s) {
  uint32_t hash = 0;
  if (AocHashmapVecStateContains(&ctx->map, ctx->carrier.position, &hash))
    AocHashmapVecStateRemovePreHashed(&ctx->map, ctx->carrier.position, hash);

  AocHashmapVecStateInsertPreHashed(&ctx->map, ctx->carrier.position, s, hash);
}

typedef state (*state_change_func)(const state);

static uint32_t solve(context *const ctx, const uint32_t bursts,
                      state_change_func change) {
  uint32_t infections = 0;
  for (uint32_t i = 0; i < bursts; ++i) {
    state s = get_state(ctx);
    ctx->carrier.dir = TURN_LOOKUP[s](ctx->carrier.dir);
    s = change(s);
    set_state(ctx, s);

    if (s == STATE_INFECTED)
      infections++;

    move(ctx);
  }
  return infections;
}

int main(void) {
  char *content = NULL;
  size_t length = 0;
  AocReadFileToString("day22/input.txt", &content, &length);

  context ctx = {0};
  AocHashmapVecStateCreate(&ctx.map, 131072);
  parse(content, &ctx);
  const uint32_t part1 = solve(&ctx, 10000, toggle_state);

  AocHashmapVecStateClear(&ctx.map);
  parse(content, &ctx);
  const uint32_t part2 = solve(&ctx, 10000000, next_state);

  printf("%u\n", part1);
  printf("%u\n", part2);

  AocHashmapVecStateDestroy(&ctx.map);
  free(content);
}
