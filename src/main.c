#include <stdio.h>
#define STAT_SUCCESS 0
#define STAT_GEN -1
#define STAT_NEA -2
#define STAT_IOFAIL -3

#define TEMP_BUFFER_SIZE 64 // NOTE: Will eventually implement a dynamic buffer

int main(int argc, char *argv[]) {
  if (argc < 1) {
    // NOTE: Eventually handle empty parameters
    fprintf(stderr, "");
    return STAT_NEA;
  }

}
