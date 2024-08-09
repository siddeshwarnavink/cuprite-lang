#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"

int main() {
  bool running = true;
  char input[50];
  while (running) {
    printf("cuprite > ");
    if (fgets(input, sizeof(input), stdin) != NULL) {
      size_t len = strlen(input);
      if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
      }

      if (strcmp(input, "exit") == 0) {
        printf("bye");
        running = false;
      } else {
        token_list list;
        token_list_create(&list);
        parse_line(&list, input);
        ast_parse_tokens(list);
        token_list_destroy(&list);
      }
    } else {
      printf("Statement too long\n");
    }
  }
  return 0;
}
