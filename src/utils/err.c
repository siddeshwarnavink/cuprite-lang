#include <complex.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "utils/memstk.h"
#include "utils/singleton.h"

SINGLETON(err, err_singleton, {false})

void err_create() {
  err *myerr = err_singleton();
  myerr->type = err_none;
  setjmp(myerr->buf);

  if (err_occurred()) {
    memstk_clean();
    exit(EXIT_FAILURE);
  }
}

bool err_occurred() {
  err *myerr = err_singleton();
  return myerr->type == err_fatal;
}

void err_throw(err_type type, char *msg) {
  err *myerr = err_singleton();
  myerr->type = type;
  switch (type) {
  case err_warning:
    printf("\033[1;33m\uea6c [Error]\033[0m %s\n", msg);
    break;
  case err_error:
    printf("\033[1;31m\uea6c [Error]\033[0m %s\n", msg);
    break;
  case err_fatal:
    printf("\033[1;31m\uea6c [Fatal error]\033[0m %s\n", msg);
    longjmp(myerr->buf, 1);
    break;
  default:
    break;
  }
}
