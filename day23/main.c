#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef union {
  int64_t data[8];
  struct {
    int64_t a;
    int64_t b;
    int64_t c;
    int64_t d;
    int64_t e;
    int64_t f;
    int64_t g;
    int64_t h;
  };
} registers;

#define INSTR_LIST(code)                                                       \
  code(nop), code(set_rr), code(set_ri), code(sub_rr), code(sub_ri),           \
      code(mul_rr), code(mul_ri), code(jnz_rr), code(jnz_ri), code(jnz_ir),    \
      code(jnz_ii)

#define INSTR_TYPE(v) instr_type_##v
#define INSTR_LABEL(v) instr_##v

typedef enum {
  INSTR_LIST(INSTR_TYPE),
} instruction_type;

typedef struct {
  instruction_type type;
  int64_t operand1;
  int64_t operand2;
} instruction;

#define AOC_T instruction
#define AOC_T_NAME Instr
#include <aoc/array.h>

static inline bool custom_isdigit(const char c) {
  return c == '-' || isdigit(c);
}

static void parse_argument(char *line, char **end, bool *const isRegister,
                           int64_t *const value) {
  if (custom_isdigit(*line)) /* number */ {
    *value = (int64_t)strtol(line, end, 10);
    *isRegister = false;
  } else /* register */ {
    *end = line + 1;
    *value = (*line) - 'a';
    *isRegister = true;
  }
}

static void parse(char *line, size_t length, void *userData) {
  AocArrayInstr *const instructions = userData;
  instruction instr = {0};

  bool isRegister1 = false;
  bool isRegister2 = false;
  int64_t value1 = 0;
  int64_t value2 = 0;

  switch (*line) {
  case 'j': /* jnz */
    line += 4;
    parse_argument(line, &line, &isRegister1, &value1);
    parse_argument(line + 1, &line, &isRegister2, &value2);
    instr.operand1 = value1;
    instr.operand2 = value2;
    instr.type = instr_type_jnz_rr;
    if (!isRegister1)
      instr.type += 2;
    if (!isRegister2)
      instr.type += 1;
    break;
  case 'm': /* mod or mul */
    line += 4;
    parse_argument(line, &line, &isRegister1, &value1);
    parse_argument(line + 1, &line, &isRegister2, &value2);
    instr.operand1 = value1;
    instr.operand2 = value2;
    instr.type = instr_type_mul_rr + (!isRegister2);
    break;
  case 's': /* set or snd */
    line++;
    if (*line == 'e') /* set */ {
      line += 3;
      parse_argument(line, &line, &isRegister1, &value1);
      parse_argument(line + 1, &line, &isRegister2, &value2);
      instr.operand1 = value1;
      instr.operand2 = value2;
      instr.type = instr_type_set_rr + (!isRegister2);
    } else /* sub */ {
      line += 3;
      parse_argument(line, &line, &isRegister1, &value1);
      parse_argument(line + 1, &line, &isRegister2, &value2);
      instr.operand1 = value1;
      instr.operand2 = value2;
      instr.type = instr_type_sub_rr + (!isRegister2);
    }
    break;
  }

  AocArrayInstrPush(instructions, instr);
}

static uint32_t solve_part1(const AocArrayInstr *const instructions) {
  registers r = {0};
  instruction *const instr = instructions->items;
  uint32_t mulInvocations = 0;

  register int64_t pc = -1;

  static void *dispatchTable[] = {
      INSTR_LIST(&&INSTR_LABEL),
  };

#define DISPATCH() goto *dispatchTable[instr[++pc].type];

  DISPATCH();
  for (;;) {
    INSTR_LABEL(nop) : return mulInvocations;
    INSTR_LABEL(set_rr) : {
      r.data[instr[pc].operand1] = r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(set_ri) : {
      r.data[instr[pc].operand1] = instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(sub_rr) : {
      r.data[instr[pc].operand1] -= r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(sub_ri) : {
      r.data[instr[pc].operand1] -= instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(mul_rr) : {
      mulInvocations++;
      r.data[instr[pc].operand1] *= r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(mul_ri) : {
      mulInvocations++;
      r.data[instr[pc].operand1] *= instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(jnz_rr) : {
      if (r.data[instr[pc].operand1] > 0)
        pc += r.data[instr[pc].operand2] - 1;
      DISPATCH();
    }
    INSTR_LABEL(jnz_ri) : {
      if (r.data[instr[pc].operand1] != 0)
        pc += instr[pc].operand2 - 1;
      DISPATCH();
    }
    INSTR_LABEL(jnz_ir) : {
      if (instr[pc].operand1 != 0)
        pc += r.data[instr[pc].operand2] - 1;
      DISPATCH();
    }
    INSTR_LABEL(jnz_ii) : {
      if (instr[pc].operand1 != 0)
        pc += instr[pc].operand2 - 1;
      DISPATCH();
    }
  }

#undef DISPATCH
  return -1; // should never reach
}

int main(void) {
  AocArrayInstr instructions = {0};
  AocArrayInstrCreate(&instructions, 40);
  AocReadFileLineByLine("day23/input.txt", parse, &instructions);
  AocArrayInstrPush(&instructions, (instruction){.type = instr_type_nop});

  const uint32_t part1 = solve_part1(&instructions);

  printf("%u\n", part1);

  AocArrayInstrDestroy(&instructions);
}
