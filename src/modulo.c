#include "modulo.h"

unsigned long modulo(__int128_t number, unsigned long base) {
  return number >= 0 ? number % base : ((number % base) + base);
}