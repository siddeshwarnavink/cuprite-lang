#ifndef MEMSTK_H
#define MEMSTK_H

#include <glib.h>

/**
 * @file memstk.h
 * @brief DIY garbage collector
 */

typedef void (*cleanup_func)(void *);

typedef struct memstk_node {
  void **resource;
  cleanup_func cleanup;
} memstk_node;

typedef struct sMemstk {
  GQueue *resource_stack;
} memstk;

void memstk_init();

void memstk_push(void **resource, cleanup_func cleanup);

void memstk_clean();

#endif
