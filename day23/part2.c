// translated from the assembly input and then improved in several steps

#include <stdio.h>

static int is_prime(int n) {
  if (n <= 1)
    return 0;

  for (int i = 2; i * i <= n; i++)
    if (n % i == 0)
      return 0;

  return 1;
}

int main(void) {
  int b = 105700;
  int c = 122700;
  int h = 0;

  for (; b <= c; b += 17)
    if (!is_prime(b))
      h += 1;

  printf("%d\n", h);
}