#include <setjmp.h>
#include <stdio.h>

#include "err.h"

err myerr;

void err_create() {
  if (!myerr.init) {
    myerr.type = err_none;
    setjmp(myerr.buf);
    myerr.init = true;
  }
}

bool err_occurred() { return myerr.type == err_fatal; }

void err_throw(err_type type, char *msg) {
  myerr.type = type;
  switch (type) {
  case err_warning:
    printf("\033[1;33m\uea6c [Error]\033[0m %s\n", msg);
    break;
  case err_error:
    printf("\033[1;31m\uea6c [Error]\033[0m %s\n", msg);
    break;
  case err_fatal:
    printf("\033[1;31m\uea6c [Fatal error]\033[0m %s\n", msg);
    break;
  default:
    break;
  }
  if (type == err_fatal) {
    longjmp(myerr.buf, 1);
  }
}

void err_destroy() { myerr.init = false; }
