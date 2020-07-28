#include <nng/nng.h>
#include <nng/supplemental/http/http.h>
#include <stdio.h>
#include <stdlib.h>

#include "Open3dStreamUtil.h"

void
fatal(int rv) {
  fprintf(stderr, "%s\n", nng_strerror(rv));
  exit(1);
}
