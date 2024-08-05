#ifndef STR_H
#define STR_H

/**
 * @file str.h
 * @brief Simple string library.
 */

typedef struct sStr {
  char *data;        /**< Actual string  */
  unsigned int size; /**< Length of string */
} *str;

/**
 * @brief Creates a new string.
 * @param s String pointer.
 * @param val Initial value.
 */
void str_create(str *s, const char *val);

/**
 * @brief Destroys a string.
 * @param s String pointer.
 */
void str_destroy(str *s);

/**
 * @brief Get the string value.
 * @param s String pointer.
 */
char *str_val(str *s);

/**
 * @brief Get the string size.
 * @param s String pointer.
 */
unsigned int str_size(str *s);

/**
 * @brief Append value at the end.
 * @param s String pointer.
 * @param val Value to append.
 */
void str_append(str *s, const char *val);

/**
 * @brief Clear contents of string.
 * @param s String pointer.
 */
void str_clear(str *s);

#endif
