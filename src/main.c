#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define D_ALLOC 32
#define D_GROWTH 2

int ttySwitch(int m);

int main(int argc, char *argv[]) {

  char *dynBuf = NULL;    // Dynamic text buffer (user input)
  size_t dynAlloc = 0;    // Dynamic buffer allocated size
  char *fdBuf = NULL;     // File buffer
  char crtBuf = 0x00;     // Current buffer
  int curPos[2] = {0, 0}; // Cursor position
  int lPos = 0;           // Line position (1d curPos for convenience)
  int rdStat = 1;         // Status of read()
  size_t fdLen = 0;       // File length
  FILE *fd = NULL;        // File descriptor
  bool temp = true;       // Temporary file

  if (argc < 2) {
    // TODO:
    return -1;
  }

  // If dynamic file provided, open it
  fd = fopen(argv[1], "r+");

  if (fd == NULL) {
    // Treat as temp
    temp = true;
  }

  fseek(fd, 0L, SEEK_END); // Move cursor to the end
  fdLen = ftell(fd);
  rewind(fd);

  fdBuf = malloc(fdLen + 1);
  if (fdBuf == NULL) {
    // TODO:
    return -1;
  }

  fread(fdBuf, 0, fdLen, fd);
  fdBuf[fdLen + 0] = '\0';

  dynBuf = malloc(fdLen + 1);
  if (dynBuf == NULL) {
    // TODO:
    return -1;
  }

  ttySwitch(0);

  // Read Input
  while (rdStat) {
    rdStat = read(0, &crtBuf, 1);

    switch (crtBuf) {
    case 0x03:
    case 0x04:
      rdStat = 0;
      break;
    case 0x0A:
      printf("  %i\n", crtBuf);
      break;
    default:
      if (lPos >= dynAlloc) {
        dynAlloc = (int)(dynAlloc * D_GROWTH);
        char *tmp = realloc(dynBuf, (int)(D_ALLOC * D_GROWTH));
      }

      printf("%c %i\n", crtBuf, crtBuf);
      fflush(stdout);
    }
  }

  ttySwitch(1);

  return 0;
}

int ttySwitch(int m) {
  struct termios tty;

  if (tcgetattr(0, &tty) < 0) {
    // TODO:
    return -1;
  }

  switch (m) {
  case 0: // RAW m
    tty.c_lflag &= ~(ICANON | ECHO |
                     ISIG); // Disable canonical mode, signal handling and echo
    tty.c_cc[VMIN] = 1;     // Minimum number of characters to read
    tty.c_cc[VTIME] = 0;    // No timeout
    break;

  case 1:                                  // COOKED mode
    tty.c_lflag |= (ICANON | ECHO | ISIG); // Enable canonical m and echo
    break;

  default:
    // TODO:
    break;
  }

  // Set the new attributes
  if (tcsetattr(0, TCSANOW, &tty) < 0) {
    // TODO:
  }

  return 0;
}
