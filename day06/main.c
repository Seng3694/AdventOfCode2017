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

#define AOC_KEY_T banks
#define AOC_KEY_T_NAME Banks
const banks BANKS_EMPTY = {0};
#define AOC_KEY_T_EMPTY BANKS_EMPTY
#define AOC_KEY_T_HFUNC calculate_banks_hash
#define AOC_KEY_T_EQUALS banks_equals
#define AOC_VALUE_T uint32_t
#define AOC_VALUE_T_NAME U32
#define AOC_SIZE_T uint32_t
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

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

static void redistribute(uint8_t *const data, const uint8_t index) {
  uint8_t range = data[index];
  uint8_t dist = 1;
  uint8_t rest = 0;
  if (data[index] > BANKS_COUNT) {
    range = BANKS_COUNT;
    dist = data[index] / (BANKS_COUNT);
    rest = data[index] % (BANKS_COUNT);
  }
  data[index] = 0;
  data[(index + 1) & (BANKS_COUNT - 1)] += rest;
  for (uint8_t i = 0; i < range; ++i) {
    data[(index + i + 1) & (BANKS_COUNT - 1)] += dist;
  }
}

static void solve(banks *const b, uint32_t *const part1,
                  uint32_t *const part2) {
  AocHashmapBanksU32 states = {0};
  AocHashmapBanksU32Create(&states, 16384);
  uint8_t *const data = b->data;

  for (;;) {
    const uint8_t index = find_index_of_biggest(data);
    redistribute(data, index);

    banks b = {0};
    memcpy(&b.data, data, sizeof(uint8_t) * BANKS_COUNT);
    uint32_t hash = 0;
    if (AocHashmapBanksU32Contains(&states, b, &hash)) {
      *part1 = states.count + 1;
      uint32_t previousIndex = 0;
      AocHashmapBanksU32GetPrehashed(&states, b, hash, &previousIndex);
      *part2 = states.count - previousIndex;
      goto finish;
    }
    AocHashmapBanksU32InsertPreHashed(&states, b, states.count, hash);
  }

finish:
  AocHashmapBanksU32Destroy(&states);
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day06/input.txt", &input, &length);
  banks b = {0};
  parse(input, &b);

  uint32_t part1 = 0;
  uint32_t part2 = 0;
  solve(&b, &part1, &part2);

  printf("%u\n", part1);
  printf("%u\n", part2);
}
