#ifndef ERR_H
#define ERR_H

#include <setjmp.h>
#include <stdbool.h>

/**
 * @file err.h
 * @brief Global error handling library.
 */

typedef enum { err_none, err_warning, err_error, err_fatal } err_type;

typedef struct sErr {
  jmp_buf buf;
  err_type type;
} err;

/**
 * @brief Creates a error instance.
 * @param e err pointer.
 */
void err_create();

/**
 * @brief Check if error was thrown
 * @param e err pointer.
 */
bool err_occurred();

/**
 * @brief Creates a error instance.
 * @param e err instance.
 * @param type Type of error
 * @param msg Error message
 */
void err_throw(err_type type, char *msg);

#endif
