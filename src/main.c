#include "include/status.h"
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define D_ALLOC 32
#define D_GROWTH 2

const char *colour[] = {
    // Will probably move it somewhere else, eventually, hopefully
    "\033[0;34m", // Blue
    "\033[0;32m", // Green
    "\033[0;33m", // Yellow
    "\033[0;31m", // Red
    "\033[0;35m", // Purple
    "\033[0;37m", // White
    "\033[0;36m", // Cyan
    "\033[0m"     // Reset
};

typedef enum { // Log levels
  DEBUG,
  INFO,
  WARNING,
  ERROR
} LogLevel;

typedef enum { // vim modes
  NORMAL,
  INSERT,
  COMMAND,
  VISUAL
} Mode;

int ttySwitch(int m);
int ltf(char *msg, LogLevel l);

int main(int argc, char *argv[]) {

  // Variables
  char *dynTxtBuf = NULL;    // Dynamic text buffer (user input)
  size_t dynTxtAlloc = 0;    // Dynamic buffer allocated size
  char *fdBuf = NULL;     // File buffer
  char crtBuf = 0x00;     // Current buffer (last typed char)
  Mode mode = NORMAL;     // Current mode
  int curPos[2] = {0, 0}; // Cursor position (x, y)
  int lPos = 1;           // Line position (1d curPos for convenience)
  int rdStat = 1;         // Status of read()
  size_t fdLen = 0;       // File length
  FILE *fd = NULL;        // File descriptor
  bool temp = true;       // Temporary file

  // Checks
  if (argc < 2) {
    ltf("Not enough arguments\n", ERROR);
    return S_NEARG;
  }

  // If dynamic file provided, open it
  fd = fopen(argv[1], "r+");

  if (fd == NULL) {
    // Treat as temp
    temp = true;
  }

  // If file exists, read it
  fseek(fd, 0L, SEEK_END); // Move cursor to the end
  fdLen = ftell(fd);
  rewind(fd);

  fdBuf = malloc(fdLen + 1);
  if (fdBuf == NULL) {
    ltf("Memory allocation failed\n", ERROR);
    return S_MALLOC;
  }

  fread(fdBuf, 0, fdLen, fd);
  fdBuf[fdLen + 0] = '\0';

  dynTxtBuf = malloc(dynTxtAlloc + 1);
  if (dynTxtBuf == NULL) {
    ltf("Memory allocation failed\n", ERROR);
    return S_MALLOC;
  }

  ttySwitch(0);
  printf("\033c"); // Clear screen
  fflush(stdout);

  // Read Input
  while (rdStat == 1) {
    rdStat = read(0, &crtBuf, 1);

    switch(mode) {
      case NORMAL:
        switch(crtBuf) {
          case 0x0A:
          case 'h':
          case 'j':
          case 'k':
          case 'l':
          case 'i':
          case 'v':
          break;
          case ':':
            mode = COMMAND;
          break;
        }
      case INSERT:
        
      case COMMAND:
      case VISUAL:
        break;
    }

  }

  ttySwitch(1);

  return 0;
}

int ttySwitch(int m) {
  struct termios tty;

  if (tcgetattr(0, &tty) < 0) {
    ltf("Terminal attributes failed\n", ERROR);
    return S_FERR;
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
    ltf("Terminal switch failed\n", ERROR);
    break;
  }

  // Set the new attributes
  if (tcsetattr(0, TCSANOW, &tty) < 0) {
    ltf("Terminal attributes failed\n", ERROR);
    return S_FERR;
  }

  return 0;
}

int ltf(char *msg, LogLevel l) {
  // Log to file and stdout
  const char *homedir = getenv("HOME");
  if (!homedir)
    homedir = getpwuid(getuid())->pw_dir; // fallback
  char path[512];
  snprintf(path, sizeof(path), "%s/.lebre.log", homedir);

  FILE *log = fopen(path, "a");
  if (log == NULL) {
    fprintf(stderr, "Log file failed");
    return S_FERR;
  }

  switch (l) {
  case DEBUG:
    // Does not write to file
    fprintf(stdout, "[DEBUG] %s", msg);
    break;
  case INFO:
    // Does not write to file
    fprintf(stdout, "[INFO] %s", msg);
    break;
  case WARNING:
    fprintf(log, "[WARNING] %s", msg);
    break;
  case ERROR:
    fprintf(log, "[ERROR] %s", msg);
    break;
  default:
    fprintf(log, "[UNKNOWN] %s", msg);
    break;
  }

  fprintf(stdout, "%s", msg); // Write to stdout
  fclose(log);
  return 0;
}
