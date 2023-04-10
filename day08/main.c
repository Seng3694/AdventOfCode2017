#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char *data;
  size_t length;
} string;

static uint32_t calc_string_hash(const string *const s) {
  uint32_t hash = 86952287;
  for (size_t i = 0; i < s->length; ++i)
    hash *= s->data[i] ^ 37157269;
  return hash;
}

static bool string_equals(const string *const a, const string *const b) {
  return (a->data == NULL && b->data == NULL) ||
         (a->data != NULL && b->data != NULL && a->length == b->length &&
          memcmp(a->data, b->data, a->length) == 0);
}

#define AOC_KEY_T string
#define AOC_KEY_T_NAME String
const string STRING_EMPTY = {0};
#define AOC_KEY_T_EMPTY STRING_EMPTY
#define AOC_KEY_T_HFUNC calc_string_hash
#define AOC_KEY_T_EQUALS string_equals
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
  AocHashmapStringU32 registerMap;
  AocArrayI32 registers;
  AocArrayInstr instructions;
} program;

static string parse_identifier(char *str, char **end) {
  string s = {.data = str};
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

static uint32_t get_identifier_key(program *const p, string s) {
  uint32_t hash = 0;
  uint32_t key = 0;
  if (AocHashmapStringU32Contains(&p->registerMap, s, &hash)) {
    AocHashmapStringU32GetPrehashed(&p->registerMap, s, hash, &key);
  } else {
    char *str = malloc(s.length + 1);
    memcpy(str, s.data, s.length);
    str[s.length] = '\0';
    s.data = str;

    key = p->registers.length;
    AocHashmapStringU32InsertPreHashed(&p->registerMap, s, p->registers.length,
                                       hash);
    AocArrayI32Push(&p->registers, 0);
  }
  return key;
}

static void parse_line(char *line, size_t length, void *userData) {
  program *const p = userData;

  // example: "bok dec -712 if cie >= -22"
  // identifier1 op operand1 IF identifier2 rel_op operand2

  const string identifier1 = parse_identifier(line, &line);
  const operation_type op = *(++line) == 'd' ? OP_DEC : OP_INC;
  const int32_t operand1 = strtol(line + 4, &line, 10);
  const string identifier2 = parse_identifier(line + 4, &line);
  const rel_op_type relOp = parse_rel_op(line + 1, &line);
  const int32_t operand2 = strtol(line + 1, NULL, 10);

  instruction instr = {
      .targetVariable = get_identifier_key(p, identifier1),
      .checkVariable = get_identifier_key(p, identifier2),
      .type = (REL_OP_COUNT * op) + relOp,
      .operand = operand1,
      .checkOperand = operand2,
  };

  AocArrayInstrPush(&p->instructions, instr);
}

static void run_program(program const *p) {
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
  }
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
  AocHashmapStringU32Create(&p.registerMap, 32);
  AocArrayI32Create(&p.registers, 32);
  AocArrayInstrCreate(&p.instructions, 1080);

  AocReadFileLineByLine("day08/input.txt", parse_line, &p);

  run_program(&p);

  const int32_t part1 = solve_part1(&p);

  printf("%d\n", part1);

  for (size_t i = 0; i < p.registerMap.capacity; ++i)
    if (p.registerMap.keys[i].data != NULL)
      free(p.registerMap.keys[i].data);

  AocHashmapStringU32Destroy(&p.registerMap);
  AocArrayI32Destroy(&p.registers);
  AocArrayInstrDestroy(&p.instructions);
}
