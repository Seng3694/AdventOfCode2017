#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BANKS_COUNT 16

typedef struct {
  uint8_t data[BANKS_COUNT];
} banks;

static inline void parse(char *input, banks *const b) {
  size_t count = 0;
  while (*input) {
    b->data[count++] = (uint8_t)strtol(input, &input, 10);
    input++; // skip tab
  }
}

static uint32_t calculate_banks_hash(const banks *const b) {
  uint32_t hash = 86952287;
  const uint8_t *const data = b->data;
  for (size_t i = 0; i < (BANKS_COUNT / 4); i += 4) {
    hash *= data[i] ^ 37157269;
    hash *= data[i + 1] ^ 44260043;
    hash *= data[i + 2] ^ 74670691;
    hash *= data[i + 3] ^ 85436957;
  }
  return hash;
}

static bool banks_equals(const banks *const a, const banks *const b) {
  for (size_t i = 0; i < BANKS_COUNT; ++i)
    if (a->data[i] != b->data[i])
      return false;
  return true;
}

#define AOC_T banks
#define AOC_T_NAME Banks
const banks BANKS_EMPTY = {0};
#define AOC_T_EMPTY BANKS_EMPTY
#define AOC_T_HFUNC calculate_banks_hash
#define AOC_T_EQUALS banks_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashset.h>

static inline int compare(const void *const a, const void *const b) {
  return ((int)*(uint8_t *)b) - ((int)*(uint8_t *)a);
}

static size_t find_index_of_biggest(const uint8_t data[const BANKS_COUNT]) {
  uint8_t biggest = data[0];
  uint8_t index = 0;
  for (uint8_t i = 1; i < BANKS_COUNT; ++i) {
    if (data[i] > biggest) {
      biggest = data[i];
      index = i;
    }
  }
  return index;
}

static uint32_t solve_part1(banks *const b) {
  AocHashsetBanks visitedStates = {0};
  AocHashsetBanksCreate(&visitedStates, 16384);
  uint8_t *const data = b->data;

  for (;;) {
    const uint8_t index = find_index_of_biggest(data);

    if (data[index] <= BANKS_COUNT) {
      const uint8_t value = data[index];
      data[index] = 0;
      for (uint8_t i = 0; i < value; ++i) {
        data[(index + i + 1) & (BANKS_COUNT - 1)] += 1;
      }
    } else {
      const uint8_t dist = data[index] / (BANKS_COUNT);
      const uint8_t rest = data[index] % (BANKS_COUNT);
      data[index] = 0;
      data[(index + 1) & (BANKS_COUNT - 1)] += (dist + rest);
      for (uint8_t i = 1; i < BANKS_COUNT; ++i) {
        data[(index + i + 1) & (BANKS_COUNT - 1)] += dist;
      }
    }

    banks b = {0};
    memcpy(&b.data, data, sizeof(uint8_t) * BANKS_COUNT);
    uint32_t hash = 0;
    if (AocHashsetBanksContains(&visitedStates, b, &hash))
      goto finish;
    AocHashsetBanksInsertPreHashed(&visitedStates, b, hash);
  }

finish:
  AocHashsetBanksDestroy(&visitedStates);
  return (uint32_t)visitedStates.count + 1;
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day06/input.txt", &input, &length);
  banks b = {0};
  parse(input, &b);

  const uint32_t part1 = solve_part1(&b);
  printf("%u\n", part1);
}
