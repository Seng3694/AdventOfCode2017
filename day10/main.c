#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

#define STRING_LENGTH 256
#define LENGTHS_MAX_COUNT 16
#define MOD_BASE2(a, b) ((a) & ((b)-1))

static void parse(char *line, uint16_t lengths[const LENGTHS_MAX_COUNT]) {
  uint32_t count = 0;
  while (*line) {
    lengths[count++] = strtol(line, &line, 10);
    line++; // , or newline
  }
}

static void reverse(uint16_t arr[const STRING_LENGTH], const int32_t from,
                    const int32_t to) {
  const int32_t distance = (int32_t)abs(to - from);
  for (int32_t i = 0; i < distance / 2; ++i) {
    const int32_t f = MOD_BASE2(from + i, STRING_LENGTH);
    const int32_t t = MOD_BASE2(to - i, STRING_LENGTH);
    const uint16_t tmp = arr[f];
    arr[f] = arr[t];
    arr[t] = tmp;
  }
}

static uint32_t solve_part1(const uint16_t lengths[const LENGTHS_MAX_COUNT]) {
  uint16_t string[STRING_LENGTH] = {0};
  for (uint16_t i = 0; i < STRING_LENGTH; ++i)
    string[i] = i;

  uint32_t current = 0;
  for (size_t i = 0; i < LENGTHS_MAX_COUNT; ++i) {
    reverse(string, current, current + lengths[i] - 1);
    current = MOD_BASE2(current + (lengths[i] + i), STRING_LENGTH);
  }

  return string[0] * string[1];
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day10/input.txt", &input, &length);
  uint16_t lengths[LENGTHS_MAX_COUNT] = {0};
  parse(input, lengths);

  uint32_t part1 = solve_part1(lengths);

  printf("%u\n", part1);

  free(input);
}
