#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

void str_create(str *s, const char *val) {
  *s = (str)malloc(sizeof(struct sStr));
  if (*s == NULL) {
    perror("Failed to allocate memory for string");
    exit(EXIT_FAILURE);
  }

  unsigned int val_len = strlen(val);

  (*s)->data = (char *)malloc(sizeof(char) * (val_len + 1));
  if ((*s)->data == NULL) {
    perror("Failed to allocate memory for string");
    free(*s);
    exit(EXIT_FAILURE);
  }

  strcpy((*s)->data, val);
  (*s)->size = val_len;
}

void str_destroy(str *s) {
  if (*s != NULL) {
    free((*s)->data);
    free(*s);
    *s = NULL;
  }
}

char *str_val(str *s) { return (*s)->data; }

unsigned int str_size(str *s) { return (*s)->size; }

void str_append(str *s, const char *val) {
  unsigned int val_len = strlen(val);

  char *new_data = realloc((*s)->data, (*s)->size + val_len * sizeof(char));
  if (new_data == NULL) {
    perror("Failed to allocate memory for appending string");
    exit(EXIT_FAILURE);
  }

  (*s)->data = new_data;
  strcat((*s)->data, val);
  (*s)->size += val_len;
}

void str_clear(str *s) {
  if ((*s)->data != NULL) {
    free((*s)->data);
    (*s)->data = NULL;
  }

  (*s)->data = (char *)malloc(sizeof(char));
  if ((*s)->data == NULL) {
    perror("Failed to allocate memory for clearing string");
    exit(EXIT_FAILURE);
  }

  (*s)->data[0] = '\0';
  (*s)->size = 0;
}
