#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core/ast.h"
#include "core/lexer.h"
#include "core/token.h"

int main() {
  bool running = true;
  char input[50];
  token_list list;
  token_list_create(&list);

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
        parse_line(&list, input);
        token eos_tok;
        token_create(&eos_tok, token_eos, NULL);
        token_list_append(&list, &eos_tok);

        for (unsigned int i = 0; i < list->size; i++) {
          token_pp(list->tokens[i]);
        }

        ast_parse_tokens(list);
        token_list_clear(&list);
      }
    } else {
      printf("Statement too long\n");
    }
  }

  token_list_destroy(&list);

  return 0;
}
