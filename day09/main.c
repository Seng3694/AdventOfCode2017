#include <aoc/aoc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void parse_garbage(char *str, char **end) {
  while (*str != '>') {
    str += (*str == '!' ? 2 : 1);
  }
  *end = str;
}

static uint32_t parse_group(char *str, char **end, uint32_t depthScore) {
  uint32_t score = depthScore;

  do {
    if (*str == ',')
      str++;
    switch (*str) {
    case '{':
      str++; // {
      score += parse_group(str, &str, depthScore + 1);
      str++; // }
      break;
    case '<':
      str++; // <
      parse_garbage(str, &str);
      str++; // >
      break;
    }
  } while (*str == ',');

  *end = str;
  return score;
}

static uint32_t solve_part1(char *str) {
  return parse_group(str, &str, 0);
}

int main(void) {
  char *input = NULL;
  size_t length = 0;
  AocReadFileToString("day09/input.txt", &input, &length);

  uint32_t part1 = solve_part1(input);

  printf("%u\n", part1);

  free(input);
}
