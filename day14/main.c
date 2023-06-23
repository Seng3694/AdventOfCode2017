#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define STRING_LENGTH 256
#define MOD_BASE2(a, b) ((a) & ((b)-1))

static void add_suffix(char *const input, size_t *length) {
  size_t c = *length;
  input[c++] = 17;
  input[c++] = 31;
  input[c++] = 73;
  input[c++] = 47;
  input[c++] = 23;
  *length = c;
}

static void reverse(uint8_t string[const STRING_LENGTH], const int32_t from,
                    const int32_t to) {
  const int32_t distance = ((int32_t)abs(to - from)) / 2 + 1;
  for (int32_t i = 0; i < distance; ++i) {
    const int32_t f = MOD_BASE2(from + i, STRING_LENGTH);
    const int32_t t = MOD_BASE2(to - i, STRING_LENGTH);
    const uint8_t tmp = string[f];
    string[f] = string[t];
    string[t] = tmp;
  }
}

static void knot(uint8_t *const lengths, const size_t count,
                 uint8_t string[const STRING_LENGTH], uint32_t *const current,
                 uint32_t *const skip) {
  uint32_t c = *current;
  uint32_t s = *skip;
  for (size_t i = 0; i < count; ++i) {
    reverse(string, c, c + lengths[i] - 1);
    c = MOD_BASE2(c + lengths[i] + s, STRING_LENGTH);
    s++;
  }
  *current = c;
  *skip = s;
}

static void create_hash(uint8_t string[const STRING_LENGTH],
                        uint8_t hash[const 32]) {
  for (size_t i = 0; i < 16; ++i) {
    uint32_t h = 0;
    for (size_t j = 0; j < 16; ++j) {
      h ^= string[i * 16 + j];
    }
    hash[i * 2] = (h & 0xf0) >> 4;
    hash[i * 2 + 1] = (h & 0x0f);
  }
}

static void knot_hash(uint8_t *const lengths, const size_t count,
                      uint8_t hash[const 32]) {
  uint8_t string[STRING_LENGTH] = {0};
  for (size_t i = 0; i < STRING_LENGTH; ++i)
    string[i] = i;

  uint32_t current = 0;
  uint32_t skip = 0;
  for (size_t r = 0; r < 64; ++r)
    knot(lengths, count, string, &current, &skip);

  create_hash(string, hash);
}

static void mark_region(uint32_t *const map, const uint32_t x, const uint32_t y,
                        const uint32_t marker) {
  if (map[y * 128 + x] != 1)
    return;

  map[y * 128 + x] = marker;

  if (x > 0)
    mark_region(map, x - 1, y + 0, marker); // left
  if (y > 0)
    mark_region(map, x + 0, y - 1, marker); // top
  if (x < 127)
    mark_region(map, x + 1, y + 0, marker); // right
  if (y < 127)
    mark_region(map, x + 0, y + 1, marker); // bottom
}

static uint32_t create_regions(uint32_t *const map) {
  // map has only ones and zeros. start marking groups with 2 and onwards
  uint32_t regionCount = 0;
  for (uint32_t y = 0; y < 128; ++y) {
    for (uint32_t x = 0; x < 128; ++x) {
      uint32_t *const current = &map[y * 128 + x];
      if (*current == 1) {
        mark_region(map, x, y, regionCount + 2); // +2 to skip 0 and 1
        regionCount++;
      }
    }
  }
  return regionCount;
}

static void solve_both(const char *const input, uint32_t *const part1,
                       uint32_t *const part2) {
  char buffer[32] = {0};
  uint8_t hashBuffer[32] = {0};
  uint32_t map[128 * 128] = {0};

  uint32_t totalUsed = 0;
  for (uint32_t row = 0; row < 128; ++row) {
    snprintf(buffer, 32, "%s-%u", input, row);
    size_t length = strlen(buffer);
    add_suffix(buffer, &length);
    knot_hash((uint8_t *)buffer, length, hashBuffer);
    for (uint8_t i = 0; i < 32; ++i) {
      for (uint8_t j = 0; j < 4; ++j) {
        const uint8_t isUsed = AOC_CHECK_BIT(hashBuffer[i], (3 - j));
        totalUsed += isUsed;
        map[row * 128 + (i * 4) + j] = isUsed;
      }
    }
  }

  *part1 = totalUsed;
  *part2 = create_regions(map);
}

int main(void) {
  const char input[] = "ffayrhll";
  uint32_t part1 = 0;
  uint32_t part2 = 0;
  solve_both(input, &part1, &part2);

  printf("%u\n", part1);
  printf("%u\n", part2);
}
