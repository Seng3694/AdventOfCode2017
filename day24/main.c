#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BRIDGES 64

typedef struct {
  uint8_t port1;
  uint8_t port2;
} bridge;

#define AOC_T bridge
#define AOC_T_NAME Bridge
#include <aoc/array.h>

static void parse(char *line, size_t length, void *userData) {
  const bridge b = {
      .port1 = (uint8_t)strtol(line, &line, 10),
      .port2 = (uint8_t)strtol(line + 1, NULL, 10),
  };
  AocArrayBridgePush(userData, b);
}

static inline bool is_starting_bridge(const bridge b) {
  return b.port1 == 0 || b.port2 == 0;
}

static inline uint64_t set_bit(const uint64_t num, const int bit) {
  return num | (1ULL << bit);
}

static inline uint64_t clear_bit(const uint64_t num, const int bit) {
  return num & (~(1ULL << bit));
}

static inline bool check_bit(const uint64_t num, const int bit) {
  return (num & (1ULL << bit)) == (1ULL << bit);
}

typedef struct {
  uint32_t strength;
  uint32_t length;
} bridge_info;

typedef bool (*comparator_func)(const bridge_info, const bridge_info);

static inline bool is_strongest(const bridge_info a, const bridge_info b) {
  return a.strength > b.strength;
}

static inline bool is_longest_and_strongest(const bridge_info a,
                                            const bridge_info b) {
  return a.length > b.length ||
         (a.length == b.length && a.strength > b.strength);
}

static bridge_info build(size_t current, uint64_t availablePieces,
                         const uint8_t requiredPort, const uint32_t length,
                         comparator_func comparator,
                         const AocArrayBridge *const bridges) {
  bridge_info best = {.length = length};
  for (size_t i = 0; i < bridges->length; ++i) {
    const bridge *const b = &bridges->items[i];
    if (check_bit(availablePieces, i) &&
        (b->port1 == requiredPort || b->port2 == requiredPort)) {
      bridge_info info = build(i, clear_bit(availablePieces, (int)i),
                               b->port1 == requiredPort ? b->port2 : b->port1,
                               length + 1, comparator, bridges);
      if (comparator(info, best))
        best = info;
    }
  }
  return (bridge_info){.strength = best.strength +
                                   bridges->items[current].port1 +
                                   bridges->items[current].port2,
                       .length = best.length};
}

static uint32_t solve(const AocArrayBridge *const bridges,
                      comparator_func comparator) {
  bridge_info best = {0};
  uint64_t availablePieces = 0;
  for (size_t i = 0; i < bridges->length; ++i)
    availablePieces = set_bit(availablePieces, (int)i);

  for (size_t i = 0; i < bridges->length; ++i) {
    const bridge *const b = &bridges->items[i];
    if (is_starting_bridge(*b)) {
      bridge_info info =
          build(i, clear_bit(availablePieces, (int)i),
                b->port1 == 0 ? b->port2 : b->port1, 1, comparator, bridges);
      if (comparator(info, best))
        best = info;
    }
  }

  return best.strength;
}

int main(void) {
  AocArrayBridge bridges = {0};
  AocArrayBridgeCreate(&bridges, MAX_BRIDGES);
  AocReadFileLineByLine("day24/input.txt", parse, &bridges);

  const uint32_t part1 = solve(&bridges, is_strongest);
  const uint32_t part2 = solve(&bridges, is_longest_and_strongest);

  printf("%u\n", part1);
  printf("%u\n", part2);

  AocArrayBridgeDestroy(&bridges);
}
