#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str.h"
#include "lexer.h"
#include "token.h"

static bool _is_whitespace(char ch);
static bool _is_number(char ch);
static token_type _number_type(char *num);

void parse_line(token_list *list, char *line) {
  unsigned int line_size = strlen(line);

  str tok_str;
  str_create(&tok_str, "");

  bool reading_num = false;

  for (unsigned int i = 0; i < line_size; i++) {
    char ch = line[i];
    if (_is_whitespace(ch)) {
      continue;
    }

    str_append_ch(&tok_str, ch);
    token tok = NULL;

    // Reading number
    if (_is_number(line[i])) {
      reading_num = true;
    }
    if (reading_num) {
      if ((i + 1 < line_size && !_is_number(line[i + 1])) ||
          i + 1 == line_size) {
        char *num_str = str_val(&tok_str);
        token_type num_type = _number_type(num_str);
        token_create(&tok, num_type, num_str);
        reading_num = false;
      }
    }

    // Reading operators
    switch (line[i]) {
    case '(':
      token_create(&tok, token_oparentheses, NULL);
      break;
    case ')':
      token_create(&tok, token_cparentheses, NULL);
      break;
    case '+':
      token_create(&tok, token_plus, NULL);
      break;
    case '-':
      token_create(&tok, token_hyphen, NULL);
      break;
    case '*':
      token_create(&tok, token_asterisk, NULL);
      break;
    case '/':
      token_create(&tok, token_fslash, NULL);
      break;
    case '%':
      token_create(&tok, token_percent, NULL);
      break;
    case '=':
      token_create(&tok, token_equal, NULL);
      break;
    }

    if (tok != NULL) {
      token_pp(tok);
      token_list_append(list, &tok);
      str_clear(&tok_str);
    }
  }

  str_destroy(&tok_str);
}

static bool _is_whitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

static bool _is_number(char ch) {
  return ('0' <= ch && ch <= '9') || (ch == '.');
}

static token_type _number_type(char *num) {
  int len = strlen(num);
  bool is_float = false;
  int i = 0;

  if (num[0] == '-' || num[0] == '+') {
    i++;
  }

  for (; i < len; i++) {
    if (num[i] == '.') {
      is_float = true;
      break;
    }
  }

  if (is_float) {
    return token_num_float;
  }
  return token_num_int;
}
