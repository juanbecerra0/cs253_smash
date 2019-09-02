/*
smash    -- a basic command shell created for cs253
author   -- Juan Becerra
version  -- v4 (p7)
*/

//Constants
#define MAXLINE 4096
#define PPID getpid()

//Headers
#include "smash.h"

void sigint_handler() {
  if(getpid() != PPID) {
    exit(0);
  } else {
    signal(SIGINT, SIG_IGN);
  }
  fflush(stdin);
  fflush(stdout);
  fflush(stderr);
  printf("\n");
}

int main() {
  //Get ppid and create signal handler
  signal(SIGINT, sigint_handler);

  //Initialize history
  h = init_history(100);

  //Main input loop
  while (1) {
    char bfr[MAXLINE];
    fputs("$ ", stderr);
    while(fgets(bfr, MAXLINE, stdin) != NULL) {
      if (!strchr(bfr, '\n')) { //argument line is larger than MAXLINE
        fprintf(stderr, "Error: too many characters in argument\n");
        //Flush out remaining arguments
        int ch;
        while (((ch = getchar()) != EOF) && (ch != '\n'));
      } else {
        bfr[strlen(bfr) - 1] = '\0';   //replace newline with NUL
        executeCommand(bfr);
      }
      fputs("$ ", stderr);
    }
  }
  return 0;
}
