#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
  const char *data;
  size_t length;
} slice;

static uint32_t calc_slice_hash(const slice *const s) {
  uint32_t hash = 86952287;
  for (size_t i = 0; i < s->length; ++i)
    hash *= s->data[i] ^ 37157269;
  return hash;
}

#define AOC_KEY_T uint32_t
#define AOC_KEY_T_NAME U32
const uint32_t U32_EMPTY = 0x1deadb0b;
#define AOC_KEY_T_EMPTY U32_EMPTY
#define AOC_KEY_T_HFUNC(x) (*x)
#define AOC_KEY_T_EQUALS(a, b) (*a == *b)
#define AOC_VALUE_T uint32_t
#define AOC_VALUE_T_NAME U32
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

#define AOC_T int32_t
#define AOC_T_NAME I32
#include <aoc/array.h>

typedef enum {
  OP_INC,
  OP_DEC,
} operation_type;

typedef enum {
  REL_OP_EQ,
  REL_OP_NEQ,
  REL_OP_GT,
  REL_OP_GEQ,
  REL_OP_LT,
  REL_OP_LEQ,
  REL_OP_COUNT,
} rel_op_type;

typedef enum {
  INSTR_INC_EQ,
  INSTR_INC_NEQ,
  INSTR_INC_GT,
  INSTR_INC_GEQ,
  INSTR_INC_LT,
  INSTR_INC_LEQ,
  INSTR_DEC_EQ,
  INSTR_DEC_NEQ,
  INSTR_DEC_GT,
  INSTR_DEC_GEQ,
  INSTR_DEC_LT,
  INSTR_DEC_LEQ,
} instruction_type;

typedef struct {
  uint32_t targetVariable;
  uint32_t checkVariable;
  instruction_type type;
  int32_t operand;
  int32_t checkOperand;
} instruction;

#define AOC_T instruction
#define AOC_T_NAME Instr
#include <aoc/array.h>

typedef struct {
  AocArrayI32 registers;
  AocArrayInstr instructions;
} program;

typedef struct {
  AocHashmapU32U32 registerMap;
  program *const p;
} parsing_context;

static slice parse_identifier(char *str, char **end) {
  slice s = {.data = str};
  while (isalpha(*str))
    str++;
  s.length = str - s.data;
  *end = str;
  return s;
}

static rel_op_type parse_rel_op(char *str, char **end) {
  switch (*str) {
  case '!': // !=
    *end = str + 2;
    return REL_OP_NEQ;
  case '=': // ==
    *end = str + 2;
    return REL_OP_EQ;
  case '<': // < or <=
    if (str[1] == '=') {
      *end = str + 2;
      return REL_OP_LEQ;
    }
    *end = str + 1;
    return REL_OP_LT;
  case '>': // > or >=
    if (str[1] == '=') {
      *end = str + 2;
      return REL_OP_GEQ;
    }
    *end = str + 1;
    return REL_OP_GT;
    break;
  default:
    exit(EXIT_FAILURE);
    break;
  }
}

static uint32_t get_identifier_key(parsing_context *const ctx, slice s) {
  uint32_t hash = calc_slice_hash(&s);
  uint32_t key = 0;
  // only store hashes. actual identifiers are not needed
  if (!AocHashmapU32U32GetPrehashed(&ctx->registerMap, hash, hash, &key)) {
    key = ctx->p->registers.length;
    AocHashmapU32U32InsertPreHashed(&ctx->registerMap, hash,
                                    ctx->p->registers.length, hash);
    AocArrayI32Push(&ctx->p->registers, 0);
  }
  return key;
}

static void parse_line(char *line, size_t length, void *userData) {
  parsing_context *const ctx = userData;

  // example: "bok dec -712 if cie >= -22"
  // identifier1 op operand1 IF identifier2 rel_op operand2

  const slice identifier1 = parse_identifier(line, &line);
  const operation_type op = *(++line) == 'd' ? OP_DEC : OP_INC;
  const int32_t operand1 = strtol(line + 4, &line, 10);
  const slice identifier2 = parse_identifier(line + 4, &line);
  const rel_op_type relOp = parse_rel_op(line + 1, &line);
  const int32_t operand2 = strtol(line + 1, NULL, 10);

  instruction instr = {
      .targetVariable = get_identifier_key(ctx, identifier1),
      .checkVariable = get_identifier_key(ctx, identifier2),
      .type = (REL_OP_COUNT * op) + relOp,
      .operand = operand1,
      .checkOperand = operand2,
  };

  AocArrayInstrPush(&ctx->p->instructions, instr);
}

static int32_t run_program(program const *p) {
  int32_t biggestValue = INT32_MIN;
  int32_t *const registers = p->registers.items;
  for (size_t i = 0; i < p->instructions.length; ++i) {
    const instruction *const instr = &p->instructions.items[i];
    int32_t *const targetVar = &registers[instr->targetVariable];
    const int32_t *const checkVar = &registers[instr->checkVariable];

    switch (instr->type) {
    case INSTR_INC_EQ:
      if (*checkVar == instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_INC_NEQ:
      if (*checkVar != instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_INC_GT:
      if (*checkVar > instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_INC_GEQ:
      if (*checkVar >= instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_INC_LT:
      if (*checkVar < instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_INC_LEQ:
      if (*checkVar <= instr->checkOperand)
        *targetVar = *targetVar + instr->operand;
      break;
    case INSTR_DEC_EQ:
      if (*checkVar == instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    case INSTR_DEC_NEQ:
      if (*checkVar != instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    case INSTR_DEC_GT:
      if (*checkVar > instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    case INSTR_DEC_GEQ:
      if (*checkVar >= instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    case INSTR_DEC_LT:
      if (*checkVar < instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    case INSTR_DEC_LEQ:
      if (*checkVar <= instr->checkOperand)
        *targetVar = *targetVar - instr->operand;
      break;
    }

    if (*targetVar > biggestValue) {
      biggestValue = *targetVar;
    }
  }

  return biggestValue;
}

static int32_t solve_part1(program const *p) {
  int32_t biggest = INT32_MIN;
  for (size_t i = 0; i < p->registers.length; ++i)
    if (p->registers.items[i] > biggest)
      biggest = p->registers.items[i];

  return biggest;
}

int main(void) {
  program p = {0};
  AocArrayI32Create(&p.registers, 32);
  AocArrayInstrCreate(&p.instructions, 1080);

  parsing_context ctx = {.p = &p};
  AocHashmapU32U32Create(&ctx.registerMap, 32);

  AocReadFileLineByLine("day08/input.txt", parse_line, &ctx);
  AocHashmapU32U32Destroy(&ctx.registerMap);

  const uint32_t part2 = run_program(&p);
  const int32_t part1 = solve_part1(&p);

  printf("%d\n", part1);
  printf("%d\n", part2);

  AocArrayI32Destroy(&p.registers);
  AocArrayInstrDestroy(&p.instructions);
}
