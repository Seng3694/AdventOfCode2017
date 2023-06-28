#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef union {
  int64_t data[5];
  struct {
    int64_t a;
    int64_t b;
    int64_t f;
    int64_t i;
    int64_t p;
  };
} registers;

#define INSTR_LIST(code)                                                       \
  code(snd_r), code(snd_i), code(set_rr), code(set_ri), code(add_rr),          \
      code(add_ri), code(mul_rr), code(mul_ri), code(mod_rr), code(mod_ri),    \
      code(rcv_r), code(rcv_i), code(jgz_rr), code(jgz_ri), code(jgz_ir),      \
      code(jgz_ii)

#define INSTR_TYPE(v) instr_type_##v
#define INSTR_LABEL(v) instr_##v

#define STRINGIFY(x) #x
#define INSTR_DBG_ARR(v) [INSTR_TYPE(v)] = STRINGIFY(v)

typedef enum {
  INSTR_LIST(INSTR_TYPE),
} instruction_type;

const static char mnemonics[][32] = {
    INSTR_LIST(INSTR_DBG_ARR),
};

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
    uint64_t index = 0;
    switch (*line) {
    case 'a':
      index = 0;
      break;
    case 'b':
      index = 1;
      break;
    case 'f':
      index = 2;
      break;
    case 'i':
      index = 3;
      break;
    case 'p':
      index = 4;
      break;
    }
    *end = line + 1;
    *value = index;
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
  case 'a': /* add */
    line += 4;
    parse_argument(line, &line, &isRegister1, &value1);
    parse_argument(line + 1, &line, &isRegister2, &value2);
    instr.operand1 = value1;
    instr.operand2 = value2;
    instr.type = instr_type_add_rr + (!isRegister2);
    break;
  case 'j': /* jgz */
    line += 4;
    parse_argument(line, &line, &isRegister1, &value1);
    parse_argument(line + 1, &line, &isRegister2, &value2);
    instr.operand1 = value1;
    instr.operand2 = value2;
    instr.type = instr_type_jgz_rr;
    if (!isRegister1)
      instr.type += 2;
    if (!isRegister2)
      instr.type += 1;
    break;
  case 'm': /* mod or mul */
    line++;
    if (*line == 'o') /* mod */ {
      line += 3;
      parse_argument(line, &line, &isRegister1, &value1);
      parse_argument(line + 1, &line, &isRegister2, &value2);
      instr.operand1 = value1;
      instr.operand2 = value2;
      instr.type = instr_type_mod_rr + (!isRegister2);
    } else /* mul */ {
      line += 3;
      parse_argument(line, &line, &isRegister1, &value1);
      parse_argument(line + 1, &line, &isRegister2, &value2);
      instr.operand1 = value1;
      instr.operand2 = value2;
      instr.type = instr_type_mul_rr + (!isRegister2);
    }
    break;
  case 'r': /* rcv */
    line += 4;
    parse_argument(line, &line, &isRegister1, &value1);
    instr.operand1 = value1;
    instr.type = instr_type_rcv_r + (!isRegister1);
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
    } else /* snd */ {
      line += 3;
      parse_argument(line, &line, &isRegister1, &value1);
      instr.operand1 = value1;
      instr.type = instr_type_snd_r + (!isRegister1);
    }
    break;
  }

  AocArrayInstrPush(instructions, instr);
}

static int64_t solve_part1(const AocArrayInstr *const instructions) {
  registers r = {0};
  instruction *const instr = instructions->items;
  int64_t lastPlayedSound = 0;
  register int64_t pc = -1;

  static void *dispatchTable[] = {
      INSTR_LIST(&&INSTR_LABEL),
  };

#define DISPATCH() goto *dispatchTable[instr[++pc].type]

  DISPATCH();
  for (;;) {
    INSTR_LABEL(snd_r) : {
      lastPlayedSound = r.data[instr[pc].operand1];
      DISPATCH();
    }
    INSTR_LABEL(snd_i) : {
      lastPlayedSound = instr[pc].operand1;
      DISPATCH();
    }
    INSTR_LABEL(set_rr) : {
      r.data[instr[pc].operand1] = r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(set_ri) : {
      r.data[instr[pc].operand1] = instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(add_rr) : {
      r.data[instr[pc].operand1] += r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(add_ri) : {
      r.data[instr[pc].operand1] += instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(mul_rr) : {
      r.data[instr[pc].operand1] *= r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(mul_ri) : {
      r.data[instr[pc].operand1] *= instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(mod_rr) : {
      r.data[instr[pc].operand1] %= r.data[instr[pc].operand2];
      DISPATCH();
    }
    INSTR_LABEL(mod_ri) : {
      r.data[instr[pc].operand1] %= instr[pc].operand2;
      DISPATCH();
    }
    INSTR_LABEL(rcv_r) : {
      if (r.data[instr[pc].operand1] != 0)
        return lastPlayedSound;
      DISPATCH();
    }
    INSTR_LABEL(rcv_i) : {
      if (instr[pc].operand1 != 0)
        return lastPlayedSound;
      DISPATCH();
    }
    INSTR_LABEL(jgz_rr) : {
      if (r.data[instr[pc].operand1] > 0)
        pc += r.data[instr[pc].operand2] - 1;
      DISPATCH();
    }
    INSTR_LABEL(jgz_ri) : {
      if (r.data[instr[pc].operand1] > 0)
        pc += instr[pc].operand2 - 1;
      DISPATCH();
    }
    INSTR_LABEL(jgz_ir) : {
      if (instr[pc].operand1 > 0)
        pc += r.data[instr[pc].operand2] - 1;
      DISPATCH();
    }
    INSTR_LABEL(jgz_ii) : {
      if (instr[pc].operand1 > 0)
        pc += instr[pc].operand2 - 1;
      DISPATCH();
    }
  }

#undef DISPATCH
  return -1; // should never reach
}

int main(void) {
  AocArrayInstr instructions = {0};
  AocArrayInstrCreate(&instructions, 42);
  AocReadFileLineByLine("day18/input.txt", parse, &instructions);

  const int64_t part1 = solve_part1(&instructions);

  printf("%ld\n", part1);

  AocArrayInstrDestroy(&instructions);
}
