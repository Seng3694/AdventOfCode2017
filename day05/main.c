#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

#define AOC_T int32_t
#define AOC_T_NAME I32
#include <aoc/array.h>

static void parse_line(char *line, size_t length, void *userData) {
  AocArrayI32 *offsets = userData;
  AocArrayI32Push(offsets, strtol(line, NULL, 10));
}

static uint32_t solve_part1(AocArrayI32 *const offsets) {
  uint32_t steps = 0;
  const int32_t *end = offsets->items + offsets->length;
  register int32_t *pc = offsets->items;
  while (pc < end) {
    (*pc)++;
    pc += (*pc - 1);
    steps++;
  }
  return steps;
}

static uint32_t solve_part2(AocArrayI32 *const offsets) {
  uint32_t steps = 0;
  const int32_t *end = offsets->items + offsets->length;
  register int32_t *pc = offsets->items;
  size_t inc = 1;
  while (pc < end) {
    inc = *pc >= 3 ? -1 : 1;
    (*pc) += inc;
    pc += (*pc - inc);
    steps++;
  }
  return steps;
}

int main(void) {
  AocArrayI32 offsets = {0};
  AocArrayI32Create(&offsets, 1080);
  AocReadFileLineByLine("day05/input.txt", parse_line, &offsets);

  AocArrayI32 offsetsCopy = {0};
  AocArrayI32Duplicate(&offsetsCopy, &offsets);

  const uint32_t part1 = solve_part1(&offsets);
  const uint32_t part2 = solve_part2(&offsetsCopy);

  printf("%u\n", part1);
  printf("%u\n", part2);

  AocArrayI32Destroy(&offsets);
  AocArrayI32Destroy(&offsetsCopy);
}
