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

static uint32_t build(size_t current, uint64_t availablePieces,
                      const uint8_t requiredPort,
                      const AocArrayBridge *const bridges) {
  uint32_t strongest = 0;
  for (size_t i = 0; i < bridges->length; ++i) {
    const bridge *const b = &bridges->items[i];
    if (check_bit(availablePieces, i) &&
        (b->port1 == requiredPort || b->port2 == requiredPort)) {
      uint32_t strength =
          build(i, clear_bit(availablePieces, (int)i),
                b->port1 == requiredPort ? b->port2 : b->port1, bridges);
      if (strength > strongest)
        strongest = strength;
    }
  }
  return strongest + bridges->items[current].port1 +
         bridges->items[current].port2;
}

static uint32_t solve_part1(const AocArrayBridge *const bridges) {
  uint32_t strongest = 0;
  uint64_t availablePieces = 0;
  for (size_t i = 0; i < bridges->length; ++i)
    availablePieces = set_bit(availablePieces, (int)i);

  for (size_t i = 0; i < bridges->length; ++i) {
    const bridge *const b = &bridges->items[i];
    if (is_starting_bridge(*b)) {
      uint32_t strength = build(i, clear_bit(availablePieces, (int)i),
                                b->port1 == 0 ? b->port2 : b->port1, bridges);
      if (strength > strongest)
        strongest = strength;
    }
  }

  return strongest;
}

int main(void) {
  AocArrayBridge bridges = {0};
  AocArrayBridgeCreate(&bridges, MAX_BRIDGES);
  AocReadFileLineByLine("day24/input.txt", parse, &bridges);

  const uint32_t part1 = solve_part1(&bridges);

  printf("%u\n", part1);

  AocArrayBridgeDestroy(&bridges);
}
