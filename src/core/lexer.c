#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str.h"
#include "lexer.h"

void parse_line(char *line) {
  unsigned int line_size = strlen(line);
  token_list list;
  create_token_list(&list);

  str tok_str;
  str_create(&tok_str, "");

  for (unsigned int i = 0; i < line_size; i++) {
    token *tok;
    char *tok_ch = malloc(2 * sizeof(char));
    tok_ch[0] = line[i];
    tok_ch[1] = '\0';

    str_append(&tok_str, tok_ch);
    printf("%s\n", str_val(&tok_str));

    free(tok_ch);
  }

  str_destroy(&tok_str);
  destroy_token_list(&list);
}
