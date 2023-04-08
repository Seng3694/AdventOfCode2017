#include <aoc/aoc.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *start;
  size_t length;
} token;

typedef struct {
  token tokens[12];
  size_t tokenCount;
  uint32_t part1;
  uint32_t part2;
} context;

static void tokenize(const char *line, token *const buffer,
                     size_t *const count) {
  *count = 0;
  do {
    token current = {.start = line};
    while (*line && !isspace(*line)) {
      line++;
    }
    current.length = line - current.start;
    buffer[(*count)++] = current;
    line++;
  } while (*line);
}

static uint32_t calc_str_comb_hash(const char *const str, const size_t length) {
  size_t hash = 0;
  for (size_t i = 0; i < length; ++i)
    hash ^= ((uint32_t)str[i] * 77675651);
  return (50836909 * hash) ^ (68510521 * length);
}

static inline int combination_cmp(const void *const a, const void *const b,
                                  const size_t length) {
  return (int64_t)calc_str_comb_hash(a, length) -
         (int64_t)calc_str_comb_hash(b, length);
}

typedef int (*row_comparator)(const void *const a, const void *const b,
                              const size_t length);

static bool check_row(const token *const tokens, const size_t tokenCount,
                      row_comparator comparator) {
  for (size_t i = 0; i < tokenCount - 1; ++i) {
    const token *const a = &tokens[i];
    for (size_t j = i + 1; j < tokenCount; ++j) {
      const token *const b = &tokens[j];
      if (a->length == b->length &&
          comparator(a->start, b->start, a->length) == 0) {
        return false;
      }
    }
  }
  return true;
}

static void parse_line(char *line, size_t length, void *userData) {
  context *ctx = userData;
  tokenize(line, ctx->tokens, &ctx->tokenCount);
  ctx->part1 += check_row(ctx->tokens, ctx->tokenCount, memcmp);
  ctx->part2 += check_row(ctx->tokens, ctx->tokenCount, combination_cmp);
}

int main(void) {
  context ctx = {0};
  AocReadFileLineByLine("day04/input.txt", parse_line, &ctx);
  printf("%u\n", ctx.part1);
  printf("%u\n", ctx.part2);
}
