#include <aoc/aoc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void parse_garbage(char *str, char **end, uint32_t *const part2) {
  while (*str != '>') {
    if (*str == '!') {
      str += 2;
    } else {
      str++;
      (*part2)++;
    }
  }
  *end = str;
}

static uint32_t parse_group(char *str, char **end, uint32_t depthScore,
                            uint32_t *const part2) {
  uint32_t score = depthScore;

  do {
    if (*str == ',')
      str++;
    switch (*str) {
    case '{':
      str++; // {
      score += parse_group(str, &str, depthScore + 1, part2);
      str++; // }
      break;
    case '<':
      str++; // <
      parse_garbage(str, &str, part2);
      str++; // >
      break;
    }
  } while (*str == ',');

  *end = str;
  return score;
}

static void solve(char *str, uint32_t *const part1, uint32_t *const part2) {
  *part1 = parse_group(str, &str, 0, part2);
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day09/input.txt", &input, &length);

  uint32_t part1 = 0;
  uint32_t part2 = 0;
  solve(input, &part1, &part2);

  printf("%u\n", part1);
  printf("%u\n", part2);

  free(input);
}
