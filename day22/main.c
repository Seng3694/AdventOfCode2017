#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

#define MAP_PART_SIZE 8

typedef struct {
  int64_t x;
  int64_t y;
} vector2i;

typedef struct map_part {
  vector2i offset;
  uint32_t data;
} map_part;

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
#define AOC_VALUE_T map_part
#define AOC_VALUE_T_NAME Map
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
  AocHashmapVecMap maps;
  map_part current;
  carrier carrier;
} context;

static void get_map_part(context *const ctx, const vector2i offset,
                         map_part *part) {
  uint32_t hash = 0;
  if (!AocHashmapVecMapContains(&ctx->maps, offset, &hash)) {
    AocHashmapVecMapInsertPreHashed(&ctx->maps, offset,
                                    (map_part){.offset = offset}, hash);
  }
  AocHashmapVecMapGetPrehashed(&ctx->maps, offset, hash, part);
}

static inline direction turn_right(const direction dir) {
  return (direction)((((uint8_t)dir) + 1) & 3);
}

static inline direction turn_left(const direction dir) {
  return (direction)((((uint8_t)dir) - 1) & 3);
}

static void move(context *const ctx) {
  vector2i newPos = ctx->carrier.position;
  switch (ctx->carrier.dir) {
  case DIRECTION_UP:
    newPos.y--;
    if (newPos.y < 0) {
      get_map_part(ctx,
                   (vector2i){
                       .x = ctx->current.offset.x,
                       .y = ctx->current.offset.y - 4,
                   },
                   &ctx->current);
      newPos.y = 3;
    }
    break;
  case DIRECTION_RIGHT:
    newPos.x++;
    if (newPos.x > 3) {
      get_map_part(ctx,
                   (vector2i){
                       .x = ctx->current.offset.x + 4,
                       .y = ctx->current.offset.y,
                   },
                   &ctx->current);
      newPos.x = 0;
    }
    break;
  case DIRECTION_DOWN:
    newPos.y++;
    if (newPos.y > 3) {
      get_map_part(ctx,
                   (vector2i){
                       .x = ctx->current.offset.x,
                       .y = ctx->current.offset.y + 4,
                   },
                   &ctx->current);
      newPos.y = 0;
    }
    break;
  case DIRECTION_LEFT:
    newPos.x--;
    if (newPos.x < 0) {
      get_map_part(ctx,
                   (vector2i){
                       .x = ctx->current.offset.x - 4,
                       .y = ctx->current.offset.y,
                   },
                   &ctx->current);
      newPos.x = 3;
    }
    break;
  }
  ctx->carrier.position = newPos;
}

static void parse(const char *const path, context *const ctx) {
  char *content = NULL;
  size_t length = 0;
  AocReadFileToString(path, &content, &length);
  // map is 25x25 big
  // start is at 12/12
  // each map part is 4x4
  // create 7x7 map parts initially
  //
  // with the start being in the 3/3 map part
  // map parts use local coords. so it maps to 0, 0

  AocHashmapVecMapCreate(&ctx->maps, 64);
  ctx->carrier = (carrier){
      .position = {0, 0},
      .dir = DIRECTION_UP,
  };

  for (uint32_t y = 0; y < 7; ++y) {
    for (uint32_t x = 0; x < 7; ++x) {
      const vector2i offset = {.x = x * 4, .y = y * 4};
      map_part part = {.offset = offset};
      for (uint32_t my = 0; my < 4; ++my) {
        for (uint32_t mx = 0; mx < 4; ++mx) {
          const uint32_t mapIndex = my * 4 + mx;
          const uint32_t sourceY = (y * 4) + my;
          const uint32_t sourceX = (x * 4) + mx;
          if (sourceY > 24 || sourceX > 24)
            continue;
          const uint32_t sourceIndex = sourceY * 26 + sourceX;
          if (content[sourceIndex] == '#')
            part.data = AOC_SET_BIT(part.data, mapIndex);
        }
      }

      AocHashmapVecMapInsert(&ctx->maps, offset, part);
    }
  }

  // get part where the start lies
  AocHashmapVecMapGet(&ctx->maps, (vector2i){.x = 12, .y = 12}, &ctx->current);

  free(content);
}

static inline bool is_infected(context *const ctx) {
  return AOC_CHECK_BIT(ctx->current.data,
                       ctx->carrier.position.y * 4 + ctx->carrier.position.x);
}

static inline void toggle_status(context *const ctx) {
  ctx->current.data = AOC_TOGGLE_BIT(
      ctx->current.data, ctx->carrier.position.y * 4 + ctx->carrier.position.x);
  AocHashmapVecMapRemove(&ctx->maps, ctx->current.offset);
  AocHashmapVecMapInsert(&ctx->maps, ctx->current.offset, ctx->current);
}

static uint32_t solve_part1(context *const ctx) {
  uint32_t infections = 0;
  for (uint32_t i = 0; i < 10000; ++i) {
    const bool isInfected = is_infected(ctx);
    ctx->carrier.dir =
        isInfected ? turn_right(ctx->carrier.dir) : turn_left(ctx->carrier.dir);
    if (!isInfected)
      infections++;

    toggle_status(ctx);

    move(ctx);
  }
  return infections;
}

int main(void) {
  context ctx = {0};
  parse("day22/input.txt", &ctx);

  const uint32_t part1 = solve_part1(&ctx);

  printf("%u\n", part1);

  AocHashmapVecMapDestroy(&ctx.maps);
}
