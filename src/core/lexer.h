#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/**
 * @file lexer.h
 */

/**
 * @brief Parese single line of code to tokens
 * @param line Line of code.
 */
void parse_line(char *line);

#endif
