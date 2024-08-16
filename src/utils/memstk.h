#ifndef MEMSTK_H
#define MEMSTK_H

#include <glib.h>
#include <stdbool.h>

/**
 * @file memstk.h
 * @brief DIY garbage collector
 */

#define MEMSTK_CLEANUP(type, destroy_func)            \
    static void _memstk_##type##_cleanup(void *itm) { \
        type i = itm;                                 \
        destroy_func(&i);                             \
    }

typedef void (*cleanup_func)(void *);

typedef struct memstk_node {
    void **resource;
    cleanup_func cleanup;
    bool freed;
} memstk_node;

typedef struct sMemstk {
    GQueue *resource_stack;
} memstk;

void memstk_init();

memstk_node *memstk_push(void **resource, cleanup_func cleanup);

void memstk_clean();

#endif
