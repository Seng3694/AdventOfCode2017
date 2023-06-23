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

static uint32_t solve_part1(const char *const input) {
  char buffer[32] = {0};
  uint8_t hashBuffer[32] = {0};

  uint32_t active = 0;
  for (uint32_t row = 0; row < 128; ++row) {
    snprintf(buffer, 32, "%s-%u", input, row);
    size_t length = strlen(buffer);
    add_suffix(buffer, &length);
    knot_hash((uint8_t *)buffer, length, hashBuffer);
    for (uint32_t i = 0; i < 32; i += 8) {
      const uint32_t part = (uint32_t)hashBuffer[i + 7] |
                            ((uint32_t)hashBuffer[i + 6] << 4) |
                            ((uint32_t)hashBuffer[i + 5] << 8) |
                            ((uint32_t)hashBuffer[i + 4] << 12) |
                            ((uint32_t)hashBuffer[i + 3] << 16) |
                            ((uint32_t)hashBuffer[i + 2] << 20) |
                            ((uint32_t)hashBuffer[i + 1] << 24) |
                            ((uint32_t)hashBuffer[i + 0] << 28);
      active += AocPopCnt32(part);
    }
  }

  return active;
}

int main(void) {
  const char input[] = "ffayrhll";

  const uint32_t part1 = solve_part1(input);

  printf("%u\n", part1);
}
