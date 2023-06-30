#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
  int32_t x;
  int32_t y;
  int32_t z;
} vector3i;

typedef struct {
  vector3i acceleration;
  vector3i velocity;
  vector3i position;
  int32_t index;
} particle;

#define AOC_T particle
#define AOC_T_NAME Particle
#include <aoc/array.h>

static vector3i parse_tuple(char *line, char **out) {
  vector3i vec = {0};
  vec.x = strtol(line, &line, 10);
  vec.y = strtol(line + 1, &line, 10);
  vec.z = strtol(line + 1, &line, 10);
  *out = line;
  return vec;
}

static void parse(char *line, size_t length, void *userData,
                  const size_t lineNumber) {
  AocArrayParticle *const particles = userData;

  particle p = {.index = (int32_t)lineNumber};
  p.position = parse_tuple(line + 3, &line);
  p.velocity = parse_tuple(line + 6, &line);
  p.acceleration = parse_tuple(line + 6, &line);

  AocArrayParticlePush(particles, p);
}

static inline int32_t vec3i_not_quite_magnitude(const vector3i vec) {
  return abs(vec.x) + abs(vec.y) + abs(vec.z);
}

static int sort_by_acceleration(const void *const a, const void *const b) {
  const particle *const pa = a;
  const particle *const pb = b;
  return vec3i_not_quite_magnitude(pa->acceleration) -
         vec3i_not_quite_magnitude(pb->acceleration);
}

static int sort_by_velocity(const void *const a, const void *const b) {
  const particle *const pa = a;
  const particle *const pb = b;
  return vec3i_not_quite_magnitude(pa->velocity) -
         vec3i_not_quite_magnitude(pb->velocity);
}

static inline int32_t taxicab_distance(const vector3i vec) {
  return (int32_t)abs(vec.x) + (int32_t)abs(vec.y) + (int32_t)abs(vec.z);
}

static int sort_by_distance(const void *const a, const void *const b) {
  const particle *const pa = a;
  const particle *const pb = b;
  return taxicab_distance(pa->position) - taxicab_distance(pb->position);
}

static uint32_t solve_part1(AocArrayParticle *particles) {
  // sort by acceleration
  qsort(particles->items, particles->length, sizeof(particle),
        sort_by_acceleration);
  uint32_t sameAcceleration = 1;
  const int32_t smallestAcceleration =
      vec3i_not_quite_magnitude(particles->items[0].acceleration);
  for (size_t i = 1; i < particles->length; ++i) {
    if (vec3i_not_quite_magnitude(particles->items[i].acceleration) ==
        smallestAcceleration)
      sameAcceleration++;
    else
      break;
  }

  // sort all the particles with the same acceleration by velocity
  qsort(particles->items, sameAcceleration, sizeof(particle), sort_by_velocity);
  uint32_t sameVelocity = 1;
  const int32_t smallestVelocity =
      vec3i_not_quite_magnitude(particles->items[0].velocity);
  for (size_t i = 1; i < sameAcceleration; ++i) {
    if (vec3i_not_quite_magnitude(particles->items[i].velocity) ==
        smallestVelocity)
      sameVelocity++;
    else
      break;
  }

  // sort all the particles with the same acceleration and velocity by the
  // distance to 0
  qsort(particles->items, sameVelocity, sizeof(particle), sort_by_distance);
  return particles->items[0].index;
}

int main(void) {
  AocArrayParticle particles = {0};
  AocArrayParticleCreate(&particles, 1000);
  AocReadFileLineByLineEx("day20/input.txt", parse, &particles);

  uint32_t part1 = solve_part1(&particles);

  printf("%u\n", part1);

  AocArrayParticleDestroy(&particles);
}
