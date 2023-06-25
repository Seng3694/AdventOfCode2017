#include <stdio.h>
#include <stdlib.h>
#include <aoc/aoc.h>

#define MOD_BASE2(a, b) ((a) & ((b)-1))

static const uint64_t FACTOR_A = 16807;
static const uint64_t FACTOR_B = 48271;

typedef struct {
  uint64_t startValueA;
  uint64_t startValueB;
} context;

static void parse(char *line, size_t length, void *userData,
                  const size_t lineNumber) {
  context *const ctx = userData;
  // 24 => skip "Generator X starts with "
  const uint64_t value = (uint64_t)strtol(line + 24, &line, 10);
  if (lineNumber == 0)
    ctx->startValueA = value;
  else
    ctx->startValueB = value;
}

static uint32_t solve_part1(const context *const ctx, const uint32_t runs) {
  uint64_t previousValueA = ctx->startValueA;
  uint64_t previousValueB = ctx->startValueB;
  uint64_t currentValueA = previousValueA;
  uint64_t currentValueB = previousValueB;
  uint32_t matchCount = 0;

  for (uint32_t i = 0; i < runs; ++i) {
    previousValueA = currentValueA;
    previousValueB = currentValueB;
    currentValueA = (previousValueA * FACTOR_A) % 2147483647;
    currentValueB = (previousValueB * FACTOR_B) % 2147483647;
    if ((currentValueA & 0xffff) == (currentValueB & 0xffff))
      matchCount++;
  }

  return matchCount;
}

static uint32_t solve_part2(const context *const ctx, const uint32_t runs) {
  uint64_t previousValueA = ctx->startValueA;
  uint64_t previousValueB = ctx->startValueB;
  uint64_t currentValueA = previousValueA;
  uint64_t currentValueB = previousValueB;
  uint32_t matchCount = 0;

  for (uint32_t i = 0; i < runs; ++i) {
    do {
      previousValueA = currentValueA;
      currentValueA = (previousValueA * FACTOR_A) % 2147483647;
    } while (MOD_BASE2(currentValueA, 4) != 0);

    do {
      previousValueB = currentValueB;
      currentValueB = (previousValueB * FACTOR_B) % 2147483647;
    } while (MOD_BASE2(currentValueB, 8) != 0);

    if ((currentValueA & 0xffff) == (currentValueB & 0xffff))
      matchCount++;
  }

  return matchCount;
}

int main(void) {
  context ctx = {0};
  AocReadFileLineByLineEx("day15/input.txt", parse, &ctx);

  const uint32_t part1 = solve_part1(&ctx, 40000000);
  const uint32_t part2 = solve_part2(&ctx, 5000000);
  printf("%u\n", part1);
  printf("%u\n", part2);
}
