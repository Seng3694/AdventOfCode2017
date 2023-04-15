#include <aoc/aoc.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node {
  struct node *nodes[16]; // analyzed. max 16
  uint16_t value;
  uint8_t count;
} node;

typedef struct {
  node nodes[2048]; // max 2000
  uint32_t count;
} context;

static void parse_line(char *line, size_t length, void *userData) {
  context *const ctx = userData;

  // example: "20 <-> 678, 1235, 1397, 1911"
  uint16_t id = strtol(line, &line, 10);
  node *const n = &ctx->nodes[id];

  // skip " <->"
  line += 4;
  // at least one number. comma separated if more
  while (*line) {
    line++; // whitespace
    id = strtol(line, &line, 10);
    node *const other = &ctx->nodes[id];
    n->nodes[n->count++] = other;
    if (n != other)
      other->nodes[other->count++] = n;
    line++; // comma or new line
  }

  ctx->count++;
}

static void sweep(node *const n, const uint16_t value) {
  n->value = value;
  for (uint8_t i = 0; i < n->count; ++i) {
    if (n->nodes[i]->value != value)
      sweep(n->nodes[i], value);
  }
}

static void solve(context *const ctx, uint32_t *const part1,
                  uint32_t *const part2) {
  uint32_t groups = 0;
  for (uint32_t i = 0; i < ctx->count; ++i) {
    if (ctx->nodes[i].value == 0) {
      sweep(&ctx->nodes[i], ++groups);
    }
  }

  uint32_t count = 0;
  for (uint32_t i = 0; i < ctx->count; ++i) {
    if (ctx->nodes[i].value == 1)
      count++;
  }

  *part1 = count;
  *part2 = groups;
}

int main(void) {
  context ctx = {0};
  AocReadFileLineByLine("day12/input.txt", parse_line, &ctx);

  uint32_t part1 = 0;
  uint32_t part2 = 0;

  solve(&ctx, &part1, &part2);

  printf("%u\n", part1);
  printf("%u\n", part2);
}
