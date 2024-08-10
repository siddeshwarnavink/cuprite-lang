#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

#include "../utils/str.h"

/**
 * @file token.h
 */

typedef enum {
  token_num_int,
  token_num_float,
  token_str,
  token_identf, /**< Variables, function name  */
  token_plus,
  token_hyphen,
  token_asterisk,
  token_fslash, /**< Forward slash  */
  token_percent,
  token_oparentheses, /**< Open parentheses  */
  token_cparentheses, /**< Close parentheses  */
  token_equal,
  token_eos, /**< End of statement  */
  token_eof  /**< End of line  */
} token_type;

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
 * @param list Token list.
 */
void token_list_create(token_list *list);

/**
 * @brief Append token to end of list.
 * @param list Token list pointer.
 * @param tok Token to append.
 */
void token_list_append(token_list *list, token *tok);

/**
 * @brief Destroy token list.
 * @param list Token list pointer.
 */
void token_list_destroy(token_list *list);

/**
 * @brief Clears the tokens in list.
 * @param list Token list pointer.
 */
void token_list_clear(token_list *list);

/**
 * @brief Create token.
 * @param list Token pointer.
 */
void token_create(token *token, token_type type, char *value);

/**
 * @brief Destroy token.
 * @param list Token pointer.
 */
void token_destroy(token *token);

/**
 * @brief Pretty-print token to stdout.
 * @param list Token pointer.
 */
void token_pp(token token);

/**
 * @brief Copy token
 * @param list Token pointer.
 */
token token_cpy(token token);
#endif
