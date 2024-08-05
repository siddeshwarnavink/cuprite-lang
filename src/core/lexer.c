#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str.h"
#include "lexer.h"
#include "token.h"

static bool _is_whitespace(char *tok_ch);
static bool _is_number(char ch);

void parse_line(char *line) {
  unsigned int line_size = strlen(line);
  token_list list;
  create_token_list(&list);

  str tok_str;
  str_create(&tok_str, "");

  bool reading_num = false;

  for (unsigned int i = 0; i < line_size; i++) {
    char *tok_ch = malloc(2 * sizeof(char));
    tok_ch[0] = line[i];
    tok_ch[1] = '\0';

    if (_is_whitespace(tok_ch)) {
      continue;
    }

    str_append(&tok_str, tok_ch);
    token tok;

    // Reading number
    if (_is_number(line[i])) {
      reading_num = true;
    }
    if (reading_num) {
      if ((i + 1 < line_size && !_is_number(line[i + 1])) ||
          (i + 1 == line_size)) {
        create_token(&tok, token_num_int, str_val(&tok_str));
        reading_num = false;
      }
    } else if (!reading_num) {
      // Reading operators
      switch (line[i]) {
      case '+':
        create_token(&tok, token_plus, NULL);
        break;
      }
    }

    if (tok != NULL) {
      token_pp(&tok);
      append_token_list(&list, &tok);
      str_clear(&tok_str);
    }

    free(tok_ch);
  }

  str_destroy(&tok_str);
  destroy_token_list(&list);
}

static bool _is_whitespace(char *tok_ch) {
  return strcmp(tok_ch, " ") == 0 || strcmp(tok_ch, "\t") == 0 ||
         strcmp(tok_ch, "\n") == 0 || strcmp(tok_ch, "\r") == 0;
}

static bool _is_number(char ch) {
  return ('0' <= ch && ch <= '9') || (ch == '.');
}
