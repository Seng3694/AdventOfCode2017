#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
  INSTRUCTION_TYPE_MOVE,
  INSTRUCTION_TYPE_WRITE,
  INSTRUCTION_TYPE_GOTO,
} instruction_type;

typedef enum {
  MOVE_DIRECTION_LEFT,
  MOVE_DIRECTION_RIGHT,
} move_direction;

typedef struct {
  instruction_type type;
  uint8_t operand;
} instruction;

typedef struct {
  uint8_t id;
  instruction ifZero[3];
  instruction ifOne[3];
} state;

#define AOC_T state
#define AOC_T_NAME State
#include <aoc/array.h>

typedef struct {
  AocArrayState states;
  uint32_t steps;
  uint8_t startState;
} context;

static void parse_header(char *content, char **out, context *const ctx) {
  // example:
  // Begin in state A.
  // Perform a diagnostic checksum after 12794428 steps.

  ctx->startState = content[15] - 'A'; // read state char
  content += 53;                       // jump to number
  ctx->steps = strtoul(content, &content, 10);
  *out = content + 8;
}

static void parse_instructions(char *content, char **out, state *const s) {
  // example
  // "  If the current value is 0:"
  // "    - Write the value 1."
  // "    - Move one slot to the right."
  // "    - Continue with state B."
  content += 26;
  uint8_t condition = strtoul(content, &content, 10);
  content += 24;
  instruction *const instr = condition == 0 ? s->ifZero : s->ifOne;
  instr[0] = (instruction){
      .type = INSTRUCTION_TYPE_WRITE,
      .operand = (uint8_t)strtoul(content, &content, 10),
  };
  content += 29;
  instr[1] = (instruction){
      .type = INSTRUCTION_TYPE_MOVE,
      .operand = *content == 'l' ? MOVE_DIRECTION_LEFT : MOVE_DIRECTION_RIGHT,
  };
  content += *content == 'l' ? 32 : 33;
  instr[2] = (instruction){
      .type = INSTRUCTION_TYPE_GOTO,
      .operand = (*content - 'A'),
  };
  *out = content + 3;
}

static state parse_state(char *content, char **out, context *const ctx) {
  // example:
  // In state A:
  //   If the current value is 0:
  //     - Write the value 1.
  //     - Move one slot to the right.
  //     - Continue with state B.
  //   If the current value is 1:
  //     - Write the value 0.
  //     - Move one slot to the left.
  //     - Continue with state F.

  state s = {0};
  content += 10;
  s.id = (*content) - 'A';
  content += 3;
  parse_instructions(content, &content, &s);
  parse_instructions(content, out, &s);

  return s;
}

static void parse(char *content, context *const ctx) {
  parse_header(content, &content, ctx);

  while (*content) {
    AocArrayStatePush(&ctx->states, parse_state(content, &content, ctx));
  }
}

static inline uint32_t calc_i32_hash(const int32_t *const i) {
  return 2166136261u ^ ((*(uint32_t *)i) * 16777619);
}

static inline bool i32_equals(const int32_t *const a, const int32_t *const b) {
  return *a == *b;
}

#define AOC_T int32_t
#define AOC_T_NAME I32
#define AOC_T_EMPTY ((int32_t){INT32_MAX})
#define AOC_T_HFUNC(x) calc_i32_hash(x)
#define AOC_T_EQUALS(a, b) i32_equals(a, b)
#define AOC_BASE2_CAPACITY
#include <aoc/hashset.h>

static size_t solve(const context *const ctx) {
  uint8_t currentStateIndex = ctx->startState;
  AocHashsetI32 hs = {0};
  AocHashsetI32Create(&hs, 8192);
  int32_t position = 0;
  uint32_t hash = 0;

  for (uint32_t i = 0; i < ctx->steps; ++i) {
    const state *const currentState = &ctx->states.items[currentStateIndex];
    const bool contains = AocHashsetI32Contains(&hs, position, &hash);
    const instruction *const instructions =
        contains ? currentState->ifOne : currentState->ifZero;
    for (uint32_t j = 0; j < 3; ++j) {
      const instruction *const instr = &instructions[j];
      switch (instr->type) {
      case INSTRUCTION_TYPE_WRITE:
        if (!contains && instr->operand == 1) {
          AocHashsetI32InsertPreHashed(&hs, position, hash);
        } else if (contains && instr->operand == 0) {
          AocHashsetI32RemovePreHashed(&hs, position, hash);
        }
        break;
      case INSTRUCTION_TYPE_MOVE:
        position +=
            (move_direction)instr->operand == MOVE_DIRECTION_LEFT ? -1 : 1;
        break;
      case INSTRUCTION_TYPE_GOTO:
        currentStateIndex = instr->operand;
        break;
      }
    }
  }

  AocHashsetI32Destroy(&hs);
  return hs.count;
}

int main(void) {
  context ctx = {0};
  AocArrayStateCreate(&ctx.states, 16);

  char *content = NULL;
  size_t length;
  AocReadFileToString("day25/input.txt", &content, &length);

  parse(content, &ctx);

  const size_t solution = solve(&ctx);

  printf("%zu\n", solution);

  free(content);
  AocArrayStateDestroy(&ctx.states);
}
