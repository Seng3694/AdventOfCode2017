#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>

#define ROWS 16
#define COLUMNS 16

typedef struct {
  int16_t data[COLUMNS];
} row_data;

static inline int compare_i16(const void *const a, const void *const b) {
  return (*(int16_t *)b) - (*(int16_t *)a);
}

static void parse_line(char *line, size_t length, void *userData,
                       const size_t lineNumber) {
  row_data *const rows = userData;
  row_data *const current = rows + lineNumber;

  for (size_t i = 0; *line; ++i) {
    current->data[i] = strtol(line, &line, 10);
    line++; // skip tab or new line
  }
  qsort(current->data, COLUMNS, sizeof(int16_t), compare_i16);
}

static uint32_t solve_part1(const row_data *const rows) {
  uint32_t solution = 0;
  for (size_t i = 0; i < ROWS; ++i) {
    solution += rows[i].data[0] - rows[i].data[COLUMNS - 1];
  }
  return solution;
}

static uint32_t solve_part2(const row_data *const rows) {
  uint32_t solution = 0;
  for (size_t r = 0; r < ROWS; ++r) {
    for (size_t i = 0; i < COLUMNS; ++i) {
      const int16_t a = rows[r].data[i];
      for (size_t j = 0; j < COLUMNS; ++j) {
        const int16_t b = rows[r].data[j];
        if (a > b && a % b == 0) {
          solution += a / b;
        }
      }
    }
  }
  return solution;
}

int main(void) {
  row_data rows[ROWS] = {0};
  if (!AocReadFileLineByLineEx("day02/input.txt", parse_line, rows))
    return EXIT_FAILURE;

  const uint32_t part1 = solve_part1(rows);
  const uint32_t part2 = solve_part2(rows);

  printf("%u\n", part1);
  printf("%u\n", part2);
}
