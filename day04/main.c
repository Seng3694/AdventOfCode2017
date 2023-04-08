#include <aoc/aoc.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *start;
  size_t length;
} token;

typedef struct {
  token tokenBuffer[12];
  size_t tokenCount;
  uint32_t part1;
} parsing_context;

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

static void parse_line(char *line, size_t length, void *userData) {
  parsing_context *ctx = userData;
  tokenize(line, ctx->tokenBuffer, &ctx->tokenCount);
  for (size_t i = 0; i < ctx->tokenCount - 1; ++i) {
    const token *const a = &ctx->tokenBuffer[i];
    for (size_t j = i + 1; j < ctx->tokenCount; ++j) {
      const token *const b = &ctx->tokenBuffer[j];
      if (a->length == b->length && memcmp(a->start, b->start, a->length) == 0)
        goto done;
    }
  }
  ctx->part1++;
done:
}

int main(void) {
  parsing_context ctx = {0};
  AocReadFileLineByLine("day04/input.txt", parse_line, &ctx);

  printf("%u\n", ctx.part1);
}
