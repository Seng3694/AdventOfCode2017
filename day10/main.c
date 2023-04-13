#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define STRING_LENGTH 256
#define MOD_BASE2(a, b) ((a) & ((b)-1))

const static char HEX_LOOKUP[] = "0123456789abcdef";

static void parse_part1(char *line, uint8_t *const lengths,
                        size_t *const count) {
  uint32_t c = 0;
  while (*line) {
    lengths[c++] = strtol(line, &line, 10);
    line++; // , or newline
  }
  *count = c;
}

static void parse_part2(char *line, uint8_t *const chars, size_t *const count) {
  size_t c = 0;
  if (line) {
    while (*line) {
      if (!isspace(*line))
        chars[c++] = (uint8_t)(*line);
      line++;
    }
  }
  chars[c++] = 17;
  chars[c++] = 31;
  chars[c++] = 73;
  chars[c++] = 47;
  chars[c++] = 23;
  *count = c;
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

static uint32_t solve_part1(uint8_t *const lengths, const size_t count) {
  uint8_t string[STRING_LENGTH] = {0};
  for (size_t i = 0; i < STRING_LENGTH; ++i)
    string[i] = i;

  uint32_t current = 0;
  uint32_t skip = 0;
  knot(lengths, count, string, &current, &skip);
  return string[0] * string[1];
}

static void create_hash(uint8_t string[const STRING_LENGTH],
                        char hash[const 32]) {
  for (size_t i = 0; i < 16; ++i) {
    uint32_t h = 0;
    for (size_t j = 0; j < 16; ++j) {
      h ^= string[i * 16 + j];
    }
    hash[i * 2] = HEX_LOOKUP[(h & 0xf0) >> 4];
    hash[i * 2 + 1] = HEX_LOOKUP[(h & 0x0f)];
  }
}

static void solve_part2(uint8_t *const lengths, const size_t count,
                        char hash[const 32]) {
  uint8_t string[STRING_LENGTH] = {0};
  for (size_t i = 0; i < STRING_LENGTH; ++i)
    string[i] = i;

  uint32_t current = 0;
  uint32_t skip = 0;
  for (size_t r = 0; r < 64; ++r)
    knot(lengths, count, string, &current, &skip);

  create_hash(string, hash);
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day10/input.txt", &input, &length);
  size_t count = 0;
  uint8_t lengths[64] = {0};

  parse_part1(input, lengths, &count);
  const uint32_t part1 = solve_part1(lengths, count);

  parse_part2(input, lengths, &count);
  char part2[32] = {0};
  solve_part2(lengths, count, part2);

  printf("%u\n", part1);
  printf("%.*s\n", 32, part2);

  free(input);
}
