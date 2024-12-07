#include "simulate.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stat simulate(struct memory* mem, int start_addr, FILE* log_file,
                     struct symbols* symbols) {
  // dummy Stat object
  struct Stat dummy_stat = {0};

  // do nothing
  return dummy_stat;
}
