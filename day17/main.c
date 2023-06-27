#include <aoc/aoc.h>
#include <stdio.h>
#include <assert.h>

typedef struct node {
  struct node *right;
  uint32_t value;
} node;

node nodeBuffer[2018] = {0};
uint32_t nodeCount = 0;

static node *new_node(void) {
  assert(nodeCount < 2018);
  node *n = &nodeBuffer[nodeCount];
  nodeCount++;
  return n;
}

static uint32_t solve_part1(const uint32_t step) {
  node *current = new_node();
  current->right = current;
  current->value = 0;

  for (uint32_t i = 0; i < 2017; ++i) {
    for (uint32_t s = 0; s < step; ++s) {
      current = current->right;
    }
    node *newNode = new_node();
    newNode->right = current->right;
    current->right = newNode;
    newNode->value = i + 1;
    current = newNode;
  }

  return current->right->value;
}

int main(void) {
  const uint32_t input = 303;

  const uint32_t part1 = solve_part1(input);
  printf("%u\n", part1);
}
