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

#define AOC_T int32_t
#define AOC_T_NAME I32
#include <aoc/array.h>

static inline int i32_compare(const void *const a, const void *const b) {
  return (*(int32_t *)a - *(int32_t *)b);
}

static uint32_t solve_part2(AocArrayParticle *const particles) {
  AocArrayI32 collisions = {0};
  AocArrayI32Create(&collisions, particles->capacity);

  const uint32_t ticks = 100; // arbitrary. with the input the actual last
                              // collision happens at tick 40
  for (uint32_t t = 0; t < ticks; ++t) {
    // check for collision
    AocArrayI32Clear(&collisions);
    for (size_t i = 0; i < particles->length - 1; ++i) {
      const particle *const a = &particles->items[i];
      for (size_t j = i + 1; j < particles->length; ++j) {
        const particle *const b = &particles->items[j];
        if (a->position.x == b->position.x && a->position.y == b->position.y &&
            a->position.z == b->position.z) {
          AocArrayI32Push(&collisions, (uint32_t)i);
          AocArrayI32Push(&collisions, (uint32_t)j);
        }
      }
    }

    // "remove" particles when they collided
    if (collisions.length > 0) {
      qsort(collisions.items, collisions.length, sizeof(uint32_t), i32_compare);
      int32_t current = -1; // for skipping duplicates
      for (size_t i = 0; i < collisions.length; ++i) {
        if (current != collisions.items[i]) {
          // move particle at the end to position to delete
          particles->items[collisions.items[i]] =
              particles->items[particles->length - 1];
          particles->length--;
          current = collisions.items[i];
        }
      }
    }

    // update movement
    for (size_t i = 0; i < particles->length; ++i) {
      particle *const p = &particles->items[i];
      p->velocity.x += p->acceleration.x;
      p->velocity.y += p->acceleration.y;
      p->velocity.z += p->acceleration.z;
      p->position.x += p->velocity.x;
      p->position.y += p->velocity.y;
      p->position.z += p->velocity.z;
    }
  }

  AocArrayI32Destroy(&collisions);
  return (uint32_t)particles->length;
}

int main(void) {
  AocArrayParticle particles = {0};
  AocArrayParticleCreate(&particles, 1000);
  AocReadFileLineByLineEx("day20/input.txt", parse, &particles);

  const uint32_t part1 = solve_part1(&particles);
  const uint32_t part2 = solve_part2(&particles);

  printf("%u\n", part1);
  printf("%u\n", part2);

  AocArrayParticleDestroy(&particles);
}
