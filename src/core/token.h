#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/str.h"

/**
 * @file token.h
 */

typedef enum { token_num_int, token_plus, token_eof } token_type;

typedef struct sToken {
  token_type type; /**< Token type  */
  str value;       /**< Token value  */
} *token;

typedef struct sToken_list {
  token *tokens;     /**< Array of tokens  */
  unsigned int size; /**< Size of array  */
} *token_list;

/**
 * @brief Creates token list.
 * @param list Token list pointer.
 */
void create_token_list(token_list *list);

/**
 * @brief Append token to end of list.
 * @param list Token list pointer.
 * @param tok Token to append.
 */
void append_token_list(token_list *list, token *tok);

/**
 * @brief Destroy token list.
 * @param list Token list pointer.
 */
void destroy_token_list(token_list *list);

/**
 * @brief Create token.
 * @param list Token pointer.
 */
void create_token(token *token, token_type type, char *value);

/**
 * @brief Pretty-print token to stdout.
 * @param list Token pointer.
 */
void token_pp(token *token);

#endif
