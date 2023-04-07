#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

#define C_TO_D(c) ((c) - '0')

static uint32_t solve_part1(const char *const input, const size_t length) {
  uint32_t solution = input[0] == input[length - 1] ? C_TO_D(input[0]) : 0;
  for (size_t i = 0; i < length - 1; ++i) {
    if (input[i] == input[i + 1])
      solution += C_TO_D(input[i]);
  }
  return solution;
}

static uint32_t solve_part2(const char *const input, const size_t length) {
  uint32_t solution = 0;
  const size_t half = length / 2;
  for (size_t i = 0; i < length; ++i) {
    const size_t next = (i + half) % length;
    if (input[i] == input[next])
      solution += C_TO_D(input[i]);
  }
  return solution;
}

int main(void) {
  char *input;
  size_t length;
  if (!AocReadFileToString("day01/input.txt", &input, &length))
    return EXIT_FAILURE;

  const uint32_t part1 = solve_part1(input, length);
  const uint32_t part2 = solve_part2(input, length);

  printf("%u\n", part1);
  printf("%u\n", part2);

  free(input);
}
