#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core/lexer.h"

int main() {
  bool running = true;
  char input[50];
  while (running) {
    printf("cuprite > ");
    scanf("%s", input);

    if (strcmp(input, "exit") == 0) {
      printf("bye");
      running = false;
    } else {
      parse_line(input);
    }
  }
  return 0;
}
