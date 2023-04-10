#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct {
  char *start;
  size_t length;
} slice;

static uint32_t calc_slice_hash(const slice *const s) {
  uint32_t hash = 86952287;
  for (size_t i = 0; i < s->length; ++i)
    hash *= s->start[i] ^ 37157269;
  return hash;
}

static inline bool slice_equals(const slice *const a, const slice *const b) {
  return (a->start == NULL && b->start == NULL) ||
         (a->length == b->length && memcmp(a->start, b->start, a->length) == 0);
}

typedef struct node {
  uint32_t weight;
  uint32_t childrenCount;
  char *name;
  struct node *parent;
  struct node *children[8];
} node;

#define AOC_KEY_T slice
#define AOC_KEY_T_NAME Slice
const slice SLICE_EMPTY = {0};
#define AOC_KEY_T_EMPTY SLICE_EMPTY
#define AOC_KEY_T_HFUNC calc_slice_hash
#define AOC_KEY_T_EQUALS slice_equals
#define AOC_VALUE_T node *
#define AOC_VALUE_T_NAME Node
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

#define AOC_T node
#define AOC_T_NAME Node
#include <aoc/array.h>

static node *node_alloc(AocArrayNode *const nodes, const slice name) {
  node n = {.name = malloc(name.length + 1)};
  memcpy(n.name, name.start, name.length);
  n.name[name.length] = '\0';
  AocArrayNodePush(nodes, n);
  return AocArrayNodeLast(nodes);
}

static void node_free(node *const n) {
  free(n->name);
}

typedef struct {
  AocHashmapSliceNode *const lookup;
  AocArrayNode *const nodes;
} parsing_context;

static node *parse_node_name(parsing_context *const ctx, char *start,
                             char **end) {
  slice name = {.start = start};
  while (*start && isalpha(*start))
    start++;
  name.length = start - name.start;

  // check if node already exists. if not create one
  uint32_t hash = 0;
  node *n = NULL;
  if (AocHashmapSliceNodeContains(ctx->lookup, name, &hash)) {
    AocHashmapSliceNodeGetPrehashed(ctx->lookup, name, hash, &n);
  } else {
    n = node_alloc(ctx->nodes, name);
    name.start = n->name;
    AocHashmapSliceNodeInsertPreHashed(ctx->lookup, name, n, hash);
  }
  *end = start;
  return n;
}

static void parse_line(char *line, size_t length, void *userData) {
  parsing_context *const ctx = userData;
  // example (part after braces optional):
  // jjnssp (1930) -> xduyex, fzzrvz, jpsne

  node *n = parse_node_name(ctx, line, &line);
  n->weight = strtol(line + 2, &line, 10);
  line += 2; // skip brace and white space

  // if arrow operator doesn't exist stop parsing
  if (*line != '-')
    return;

  // now at: "-> xduyex, fzzrvz, jpsne"
  line += 1;

  do {
    line += 2; // skip comma and white space
    node *child = parse_node_name(ctx, line, &line);
    n->children[n->childrenCount++] = child;
    child->parent = n;
  } while (*line == ',');
}

static node *find_root(const AocArrayNode *const nodes) {
  for (size_t i = 0; i < nodes->length; ++i) {
    if (nodes->items[i].parent == NULL) {
      return &nodes->items[i];
    }
  }
  return NULL;
}

static const char *solve_part1(const AocArrayNode *const nodes) {
  for (size_t i = 0; i < nodes->length; ++i) {
    if (nodes->items[i].parent == NULL) {
      return nodes->items[i].name;
    }
  }
  return NULL;
}

int main(void) {
  AocArrayNode nodes = {0};
  AocArrayNodeCreate(&nodes, 1320);
  AocHashmapSliceNode lookup = {0};
  AocHashmapSliceNodeCreate(&lookup, 2048);
  parsing_context ctx = {
      .nodes = &nodes,
      .lookup = &lookup,
  };

  AocReadFileLineByLine("day07/input.txt", parse_line, &ctx);

  const node *const root = find_root(&nodes);

  // part 1
  printf("%s\n", root->name);

  AocHashmapSliceNodeDestroy(&lookup);
  for (size_t i = 0; i < nodes.length; ++i)
    node_free(&nodes.items[i]);
  AocArrayNodeDestroy(&nodes);
}
