#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/err.h"
#include "utils/memstk.h"

MEMSTK_CLEANUP(str, str_destroy);

void str_create(str *s, const char *val) {
    *s = (str)malloc(sizeof(struct sStr));
    if (*s == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for string");
    }

    unsigned int val_len = strlen(val);

    (*s)->data = (char *)malloc(sizeof(char) * (val_len + 1));
    if ((*s)->data == NULL) {
        free(*s);
        err_throw(err_fatal, "Failed to allocate memory for string");
    }

    strcpy((*s)->data, val);
    (*s)->size = val_len;
    (*s)->memstk_node = memstk_push((void **)&(*s), _memstk_str_cleanup);
}

void str_cpy(str *s, str *val) {
    *s = (str)malloc(sizeof(struct sStr));
    if (*s == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for string");
    }

    unsigned int val_len = strlen((*val)->data);

    (*s)->data = (char *)malloc(sizeof(char) * (val_len + 1));
    if ((*s)->data == NULL) {
        free(*s);
        err_throw(err_fatal, "Failed to allocate memory for string");
    }

    strcpy((*s)->data, (*val)->data);
    (*s)->size = val_len;
    (*s)->memstk_node = memstk_push((void **)&(*s), _memstk_str_cleanup);
}

void str_destroy(str *s) {
    if (s != NULL) {
        if (*s != NULL) {
            if ((*s)->data != NULL) {
                free((*s)->data);
                (*s)->data = NULL;
            }
            (*s)->memstk_node->freed = true;
            free(*s);
            *s = NULL;
        }
    }
}
char *str_val(str *s) { return (*s)->data; }

unsigned int str_size(str *s) { return (*s)->size; }

void str_append(str *s, const char *val) {
    unsigned int val_len = strlen(val);
    char *new_data = realloc((*s)->data, (*s)->size + val_len + 1);
    if (new_data == NULL) {
        err_throw(err_fatal, "Failed to allocate memory for appending string");
    }

    (*s)->data = new_data;
    strcat((*s)->data, val);
    (*s)->size += val_len;
}

void str_append_ch(str *st, char ch) {
    str s = *st;
    s->data = realloc(s->data, sizeof(char) * (s->size + 2));
    if (s->data == NULL) {
        err_throw(err_fatal, "Failed to reallocate memory for string");
    }
    s->data[s->size] = ch;
    s->size++;
    s->data[s->size] = '\0';
}

void str_clear(str *s) {
    if (*s != NULL) {
        if ((*s)->data != NULL) {
            free((*s)->data);
            (*s)->data = NULL;
        }

        (*s)->data = (char *)malloc(sizeof(char));
        if ((*s)->data == NULL) {
            err_throw(err_fatal,
                      "Failed to allocate memory for clearing string");
        }

        (*s)->data[0] = '\0';
        (*s)->size = 0;
    }
}
