#include <aoc/aoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint16_t size;
  uint16_t data;
} pattern;

const pattern EMPTY_PATTERN = {0};

static inline uint32_t calc_pattern_hash(const pattern *const p) {
  return 54812489 * ((uint32_t)p->size ^ 95723417) *
         ((uint32_t)p->data ^ 74297323);
}

static inline bool pattern_equals(const pattern *const a,
                                  const pattern *const b) {
  return a->size == b->size && a->data == b->data;
}

static void print_pattern(const pattern p) {
  printf("size: %u, hash: %8x\n", p.size, calc_pattern_hash(&p));
  for (uint16_t y = 0; y < p.size; ++y) {
    for (uint16_t x = 0; x < p.size; ++x) {
      const uint16_t index = y * p.size + x;
      printf("%c ", AOC_CHECK_BIT(p.data, index) ? '#' : '.');
    }
    printf("\n");
  }
}

#define AOC_KEY_T pattern
#define AOC_KEY_T_NAME Pattern
#define AOC_VALUE_T pattern
#define AOC_VALUE_T_NAME Pattern
#define AOC_KEY_T_EMPTY EMPTY_PATTERN
#define AOC_KEY_T_HFUNC calc_pattern_hash
#define AOC_KEY_T_EQUALS pattern_equals
#define AOC_BASE2_CAPACITY
#include <aoc/hashmap.h>

#define SET_BIT(value, bit, c)                                                 \
  c == '#' ? AOC_SET_BIT(value, bit) : AOC_CLEAR_BIT(value, bit)

static pattern parse_pattern(char *line, char **out, const uint16_t size) {
  pattern p = {.size = size};
  const uint16_t area = size * size;
  for (uint16_t i = 0; i < area; i += size) {
    for (uint16_t j = 0; j < size; ++j) {
      p.data = SET_BIT(p.data, i + j, *(line + i + j + (i / size)));
    }
  }
  if (out != NULL)
    *out = line + (area + (size - 1));
  return p;
}

pattern buffer[8] = {0};

static pattern rotate_right(const pattern p) {
  /*
    012    630
    345 => 741
    678    852
  */
  pattern out = {.size = p.size};
  for (uint16_t y = 0; y < p.size; ++y) {
    for (uint16_t x = 0; x < p.size; ++x) {
      const uint16_t from = y * p.size + x;
      const uint16_t to = x * p.size + (p.size - y - 1);
      if (AOC_CHECK_BIT(p.data, from))
        out.data = AOC_SET_BIT(out.data, to);
    }
  }
  return out;
}

static pattern flip_horizontal(const pattern p) {
  /*
    012    678
    345 => 345
    678    012
  */
  pattern out = {.size = p.size};
  for (uint16_t y = 0; y < p.size; ++y) {
    for (uint16_t x = 0; x < p.size; ++x) {
      const uint16_t from = y * p.size + x;
      const uint16_t to = (p.size - y - 1) * p.size + x;
      if (AOC_CHECK_BIT(p.data, from))
        out.data = AOC_SET_BIT(out.data, to);
    }
  }
  return out;
}

static void generate_all_8_patterns(const pattern p, pattern b[const 8]) {
  /*
    input and 3 rotated forms
    .#.  #..  ###  .##
    ..#  #.#  #..  #.#
    ###  ##.  .#.  ..#

    flipped and 3 rotated forms
    ###  ..#  .#.  ##.
    ..#  #.#  #..  #.#
    .#.  .##  ###  #..
  */
  b[0] = p;
  b[1] = rotate_right(p);
  b[2] = rotate_right(b[1]);
  b[3] = rotate_right(b[2]);

  b[4] = flip_horizontal(p);
  b[5] = rotate_right(b[4]);
  b[6] = rotate_right(b[5]);
  b[7] = rotate_right(b[6]);
}

static void parse(char *line, size_t length, void *userData) {
  AocHashmapPatternPattern *const map = userData;
  const uint16_t fromSize = length == 21 ? 2 : 3;
  const uint16_t toSize = length == 21 ? 3 : 4;
  const pattern from = parse_pattern(line, &line, fromSize);
  const pattern to = parse_pattern(line + 4, &line, toSize);

  generate_all_8_patterns(from, buffer);

  uint32_t hash = 0;
  for (uint32_t i = 0; i < 8; ++i) {
    if (!AocHashmapPatternPatternContains(map, buffer[i], &hash)) {
      AocHashmapPatternPatternInsertPreHashed(map, buffer[i], to, hash);
    }
  }
}

typedef struct {
  char *data;
  uint16_t size;
} image;

static uint32_t count_pixels(const image *const img) {
  uint32_t count = 0;
  const uint32_t area = img->size * img->size;
  for (uint32_t i = 0; i < area; ++i)
    count += img->data[i] == '#';
  return count;
}

static void print_image(const image *const img) {
  printf("[img]: size:%u, active:%u\n", img->size, count_pixels(img));
  for (uint16_t imgY = 0; imgY < img->size; ++imgY) {
    for (uint16_t imgX = 0; imgX < img->size; ++imgX) {
      printf("%c ", img->data[imgY * img->size + imgX]);
    }
    printf("\n");
  }
  printf("\n");
}

static void init_image(image *const img) {
  img->size = 3;
  img->data = malloc(sizeof(char) * img->size * img->size);
  memcpy(img->data, ".#...####", 9);
}

static void enhance(image *const img,
                    const AocHashmapPatternPattern *const map) {
  const uint16_t size = img->size;

  const uint32_t from = size % 2 == 0 ? 2 : 3;
  const uint32_t to = size % 2 == 0 ? 3 : 4;

  const uint32_t sizeNew = img->size + (img->size / from);
  char *data = malloc(sizeof(char) * sizeNew * sizeNew);
  for (uint16_t y = 0; y < img->size; y += from) {
    for (uint16_t x = 0; x < img->size; x += from) {
      pattern sourcePattern = {.size = from};
      for (uint16_t y2 = 0; y2 < from; ++y2) {
        for (uint16_t x2 = 0; x2 < from; ++x2) {
          const uint16_t src = (y + y2) * img->size + (x + x2);
          const uint16_t dest = y2 * from + x2;
          sourcePattern.data =
              SET_BIT(sourcePattern.data, dest, img->data[src]);
        }
      }

      pattern destPattern = {.size = to};
      AocHashmapPatternPatternGet(map, sourcePattern, &destPattern);
      for (uint16_t y2 = 0; y2 < to; ++y2) {
        for (uint16_t x2 = 0; x2 < to; ++x2) {
          const uint16_t src = y2 * to + x2;
          const uint16_t dest =
              (y + (y / from) + y2) * sizeNew + (x + (x / from) + x2);
          data[dest] = AOC_CHECK_BIT(destPattern.data, src) ? '#' : '.';
        }
      }
    }
  }

  free(img->data);
  img->data = data;
  img->size = sizeNew;
}

uint32_t solve_part1(const AocHashmapPatternPattern *const map) {
  image img = {0};
  init_image(&img);

  for (uint32_t i = 0; i < 5; ++i) {
    enhance(&img, map);
  }

  const uint32_t enabledPixels = count_pixels(&img);

  free(img.data);
  return enabledPixels;
}

int main(void) {
  AocHashmapPatternPattern map = {0};
  AocHashmapPatternPatternCreate(&map, 128);
  AocReadFileLineByLine("day21/input.txt", parse, &map);

  const uint32_t part1 = solve_part1(&map);

  printf("%u\n", part1);

  AocHashmapPatternPatternDestroy(&map);
}
