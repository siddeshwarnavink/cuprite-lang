#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str.h"
#include "lexer.h"
#include "token.h"

static bool _is_whitespace(char ch);
static bool _is_number(char ch);
static bool _is_operator(char ch);
static token_type _number_type(char *num);
static void _remove_first_char(char *str);

void parse_line(token_list *list, char *line) {
  unsigned int line_size = strlen(line);

  str tok_str;
  str_create(&tok_str, "");

  bool reading_num = false;
  bool reading_idetf = false;
  bool reading_str = false;

  for (unsigned int i = 0; i < line_size; i++) {
    char ch = line[i];
    if (!reading_str && _is_whitespace(ch)) {
      continue;
    }

    str_append_ch(&tok_str, ch);
    token tok = NULL;

    // Reading number
    if ((_is_number(ch) || reading_num) && !reading_idetf && !reading_str) {
      reading_num = true;
      if ((i + 1 < line_size && !_is_number(line[i + 1])) ||
          (reading_num && i + 1 == line_size)) {
        char *num_str = str_val(&tok_str);
        token_type num_type = _number_type(num_str);
        token_create(&tok, num_type, num_str);
        reading_num = false;

        token_list_append(list, &tok);
        str_clear(&tok_str);
      }
      continue;
    }

    // Reading operators
    if (_is_operator(ch) && !reading_str) {
      switch (ch) {
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
      case '\n':
        token_create(&tok, token_eos, NULL);
        break;
      }

      token_list_append(list, &tok);
      str_clear(&tok_str);
      continue;
    }
    // Read identifiers
    if ((!_is_number(ch) && !_is_operator(ch) && ch != '"' && !reading_str) ||
        reading_idetf) {
      reading_idetf = true;
      if ((i + 1 < line_size && _is_operator(line[i + 1])) ||
          (reading_idetf && i + 1 == line_size)) {
        char *idetf_str = str_val(&tok_str);
        token_create(&tok, token_identf, idetf_str);
        reading_idetf = false;

        token_list_append(list, &tok);
        str_clear(&tok_str);
      }
      continue;
    }

    // Reading string
    if (ch == '"' || reading_str) {
      reading_str = true;
      if ((i + 1 < line_size && line[i + 1] == '"') ||
          (reading_idetf && i + 1 == line_size)) {
        char *str_str = str_val(&tok_str);
        _remove_first_char(str_str);
        token_create(&tok, token_str, str_str);
        reading_str = false;

        token_list_append(list, &tok);
        str_clear(&tok_str);
      }
      continue;
    }
  }

  str_destroy(&tok_str);
}

static bool _is_whitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\r';
}

static bool _is_number(char ch) {
  return ('0' <= ch && ch <= '9') || (ch == '.');
}

static bool _is_operator(char ch) {
  return ch == '(' || ch == ')' || ch == '+' || ch == '-' || ch == '*' ||
         ch == '/' || ch == '%' || ch == '=' || ch == '\n';
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

static void _remove_first_char(char *str) {
  if (str != NULL && strlen(str) > 0) {
    // Shift all characters to the left by one
    for (int i = 0; str[i] != '\0'; i++) {
      str[i] = str[i + 1];
    }
  }
}
