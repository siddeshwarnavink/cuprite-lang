#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "memstk.h"
#include "utils/singleton.h"

SINGLETON(memstk, memstk_singleton, {NULL})

static void _clean_node(void *n);

void memstk_init() {
  memstk *stk = memstk_singleton();
  printf("stack created %p\n", stk);
  if (stk->resource_stack == NULL) {
    stk->resource_stack = g_queue_new();
  }
}

memstk_node *memstk_push(void **resource, cleanup_func cleanup) {
  memstk *stk = memstk_singleton();
  if (stk->resource_stack == NULL) {
    memstk_init();
  }

  memstk_node *node = malloc(sizeof(memstk_node));
  node->resource = resource;
  node->cleanup = cleanup;
  node->freed = false;
  g_queue_push_tail(stk->resource_stack, node);

  printf("pushed %p to stack %p\n", resource, stk);
  return node;
}

void memstk_clean() {
  memstk *stk = memstk_singleton();
  printf("cleaning up stack %p ...\n", stk);
  if (stk->resource_stack != NULL) {
    g_queue_free_full(stk->resource_stack, _clean_node);
    stk->resource_stack = NULL;
  }
}

static void _clean_node(void *n) {
  memstk_node *node = n;
  if (node != NULL) {
    if (!node->freed) {
      printf("cleaning up %p\n", node->resource);
      if (node->cleanup) {
        node->cleanup(*node->resource);
      } else {
        free(*node->resource);
      }
      *node->resource = NULL;
    }
    free(node);
  }
}
