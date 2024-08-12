#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON(t, inst, init)                                               \
  t *t##_singleton() {                                                         \
    static t inst = init;                                                      \
    return &inst;                                                              \
  }

#endif
