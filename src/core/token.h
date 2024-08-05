#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/str.h"

/**
 * @file token.h
 */

typedef enum eToken_type { token_num_int, token_plus } token_type;

typedef struct sToken {
  token_type type; /**< Token type  */
  str value;       /**< Token value  */
} *token;

typedef struct sToken_list {
  token *tokens;     /**< Array of tokens  */
  unsigned int size; /**< Size of array  */
} *token_list;

void create_token_list(token_list *list);

void append_token_list(token_list *list, token *tok);

void destroy_token_list(token_list *list);

void create_token(token *token, token_type type, char *value);

#endif
