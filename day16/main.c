#include <aoc/aoc.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  INSTR_SPIN,
  INSTR_EXCHANGE,
  INSTR_PARTNER,
} instruction_type;

typedef struct {
  instruction_type type;
  union {
    struct {
      uint8_t amount;
    } spin;
    struct {
      uint8_t position1;
      uint8_t position2;
    } exchange;
    struct {
      char program1;
      char program2;
    } partner;
  };
} instruction;

#define AOC_T instruction
#define AOC_T_NAME Instr
#include <aoc/array.h>

static void parse(const char *const path, AocArrayInstr *const instructions) {
  char *contents = NULL;
  size_t length = 0;
  AocReadFileToString(path, &contents, &length);
  char *input = contents;

  // formats
  //  x1/14
  //  s12
  //  pb/f
  while (*input != '\0') {
    const char command = *input;
    input++;
    instruction instr = {0};

    switch (command) {
    case 'x':
      instr.type = INSTR_EXCHANGE;
      instr.exchange.position1 = strtol(input, &input, 10);
      instr.exchange.position2 = strtol(input + 1, &input, 10);
      break;
    case 's':
      instr.type = INSTR_SPIN;
      instr.spin.amount = strtol(input, &input, 10);
      break;
    case 'p':
      instr.type = INSTR_PARTNER;
      instr.partner.program1 = *input;
      input += 2;
      instr.partner.program2 = *input;
      input++;
      break;
    default:
      input++;
      continue;
    }

    AocArrayInstrPush(instructions, instr);
    input++;
  }

  free(contents);
}

static void spin(char *const programs, uint8_t amount) {
  for (uint8_t i = 0; i < amount; ++i) {
    const char tmp = programs[15];
    for (uint8_t j = 15; j > 0; --j) {
      programs[j] = programs[j - 1];
    }
    programs[0] = tmp;
  }
}

static void exchange(char *const programs, const uint8_t position1,
                     const uint8_t position2) {
  const char tmp = programs[position1];
  programs[position1] = programs[position2];
  programs[position2] = tmp;
}

static void partner(char *const programs, const char program1,
                    const char program2) {
  uint8_t pos1 = 0;
  uint8_t pos2 = 0;
  for (uint8_t i = 0; i < 16; ++i) {
    if (programs[i] == program1)
      pos1 = i;
    if (programs[i] == program2)
      pos2 = i;
  }

  exchange(programs, pos1, pos2);
}

static void solve_part1(char *const programs,
                        const AocArrayInstr *const instructions) {
  for (size_t i = 0; i < instructions->length; ++i) {
    const instruction *const instr = &instructions->items[i];
    switch (instr->type) {
    case INSTR_SPIN:
      spin(programs, instr->spin.amount);
      break;
    case INSTR_EXCHANGE:
      exchange(programs, instr->exchange.position1, instr->exchange.position2);
      break;
    case INSTR_PARTNER:
      partner(programs, instr->partner.program1, instr->partner.program2);
      break;
    }
  }
}

static void solve_part2(char *const programs,
                        const AocArrayInstr *const instructions) {
  const char *start = "abcdefghijklmnop";
  uint32_t runs = 0;
  // start at 2 because initial value and one dance is already done
  // search for repeating pattern
  for (uint32_t i = 2;; ++i) {
    solve_part1(programs, instructions);
    if (strcmp(programs, start) == 0) {
      // i is the cycle length. find required runs for the solution
      runs = 1000000000 % i;
      break;
    }
  }

  for (uint32_t i = 0; i < runs; ++i)
    solve_part1(programs, instructions);
}

int main(void) {
  AocArrayInstr instructions = {0};
  AocArrayInstrCreate(&instructions, 10000);
  parse("day16/input.txt", &instructions);

  char programs[17] = "abcdefghijklmnop";

  solve_part1(programs, &instructions);
  printf("%s\n", programs);

  solve_part2(programs, &instructions);
  printf("%s\n", programs);

  AocArrayInstrDestroy(&instructions);
}
