//Imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TOKEN_BUFFER 128
#define ARGV_BUFFER 1024

#include "smash.h"

//Method signatures
void executeExternalCommand();
void exit_smash();

int has_malloc = 0;
char** argv;

//p4.5
//Parses, tokenizes, and executes command line arguments
void executeCommand(char* str) {
  char* token = strtok(str," "); 

  if(has_malloc == 0) {
    argv = (char**) malloc(ARGV_BUFFER * sizeof(char*));
    if(argv == NULL) {
      printf("MALLOC ERROR\n");
      exit(1);
    }
    for(int j = 0; j < ARGV_BUFFER; j++) {
      argv[j] = (char*) malloc(sizeof(char) * CHAR_BUFFER);
      if(argv[j] == NULL) {
        printf("MALLOC ERROR\n");
        exit(1);
      }
      strncpy(argv[j], "", CHAR_BUFFER);
    }
    has_malloc = 1;
  }

  int i = 0;
  //Tokenize entire input str and put into argv array
  while(token != NULL) {
    strncpy(argv[i], token, CHAR_BUFFER);
    token = strtok(NULL, " ");
    i++;
  }

  //Token selection
  //---------------------------------------------------------------------------------------------
  // *** exit -- exit program
  if(strncmp(argv[0], "exit", TOKEN_BUFFER) == 0) {
    exit_smash();
  //---------------------------------------------------------------------------------------------
  // *** cd [directory] -- change directory *** has ONE args
  } else if(strncmp(argv[0], "cd", TOKEN_BUFFER) == 0) {
    if(strncmp(argv[1], "", TOKEN_BUFFER) == 0 || strncmp(argv[1], "~", TOKEN_BUFFER) == 0) { //if cd [NULL] or cd ~, go home
      int pid = fork();
      if (pid == 0) {
        //Go to home directory
        chdir("/home");
        exit(0);
      } else {
        add_history(h, argv[0], 0, argv);
        wait(NULL);
      }
    } else if(strncmp(argv[1], "..", TOKEN_BUFFER) == 0) {  //Go to previous directory
      int pid = fork();
      if (pid == 0) {
        //Go to previous directory
        chdir("..");
        exit(0);
      } else {
        add_history(h, argv[0], 0, argv);
        wait(NULL);
      }
    } else {
      DIR* dir = opendir(argv[1]);
      if(dir) {  //Directory exists
        int pid = fork();
        if(pid == 0) {
          //Go to directory
          chdir(argv[1]);
          exit(0);
        } else {
          add_history(h, argv[0], 0, argv);
          wait(NULL);
        }
      } else {  //Directory does not exist
        int pid = fork();
        if(pid == 0) {
          //Print an error message
          perror(argv[1]);
          exit(0);
        } else {
          add_history(h, argv[0], 1, argv);
          wait(NULL);
        }    
      }
    }

    //Flush hanging arguments
    if (strncmp(argv[2], "", TOKEN_BUFFER) != 0) {
      fprintf(stderr, "NOTE: Arguments after '%s' where ignored.\n", argv[1]);
    }
  //---------------------------------------------------------------------------------------------
  // *** history -- prints history to console *** Has no args
  } else if (strncmp(argv[0], "history", TOKEN_BUFFER) == 0) {
    add_history(h, argv[0], 0, argv);
    print_history(h);
    if (strncmp(argv[1], "", TOKEN_BUFFER) != 0) {
      fprintf(stderr, "NOTE: Arguments after 'history' where ignored.\n");
    }
  //---------------------------------------------------------------------------------------------
  // *** exec -- executes the command and arguments
  } else {
    executeExternalCommand();
  }
  //---------------------------------------------------------------------------------------------
  //Populate argv with empty strings
  for(int j = 0; j < ARGV_BUFFER; j++) {
    strncpy(argv[j], "", TOKEN_BUFFER);
  }
}

//Executes an external command based on argv
//Prints error message if command does not exist
void executeExternalCommand() {
  //Save I/O in case of I/O redirection
  int s_in = dup(0);
  int s_out = dup(1);

  //Initialize iterator
  int i = 0;
  int pipe_flag = -1;
  int fail_flag = 0;

  //Count the amount of arguments in the command
  //7.1 also, check for I/O redirection or pipe operations
  while(strncmp(argv[i], "", TOKEN_BUFFER) != 0) {
    int shift_flag = 0;
    i++;

    if (strncmp(argv[i], "|", TOKEN_BUFFER) == 0) {
      //Pipe operator found (only one can be used)
      if(pipe_flag == -1) {
        pipe_flag = i;
      } else {
        fprintf(stderr, "ERROR: Only one pipe is allowed for a single command.\n");

        //Add failed command to history
        add_history(h, argv[0], 1, argv);
        fail_flag = 1;
        break;
      }
    } else if(strncmp(argv[i], "<", TOKEN_BUFFER) == 0) {
      //Create a new input file descriptor
      int in = open(argv[i + 1], O_RDONLY);

      //Replace standard input with input file
      dup2(in, 0);

      //Close file descriptor
      close(in);
      shift_flag = 1;
    } else if(strncmp(argv[i], ">", TOKEN_BUFFER) == 0) {
      //Create a new output file descriptor
      int out = open(argv[i + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

      //Replace standart output with output file
      dup2(out, 1);

      //Close file descriptor
      close(out);
      shift_flag = 1;
    }

    //If IO was redirected, shift arguments forward to remove < or >
    if(shift_flag == 1) {
      int j = i;

      //Since a redirector requires two arguments, pull argv[j+2] forward
      while(strncmp(argv[j], "", TOKEN_BUFFER) != 0) {
        strncpy(argv[j], argv[j + 2], TOKEN_BUFFER);
        j++;
      }
      i--;
    }
  }
 
  //Normal command
  if(pipe_flag == -1) {
    //Save address of argv[i] and set as null determinant
    char* temp_ptr = argv[i];
    argv[i] = NULL;

    //Execute the external command by replacing this process with it
    int pid = fork();
    if(pid == 0) {
      int r = execvp(argv[0], argv);
      if(r < 0) {
        fprintf(stderr, "ERROR: %s is not a valid executable\n", argv[0]);
        exit(127);
      } else {
        exit(0);
      }  
    } else {
      int exitStatus = -1;
      wait(&exitStatus);
    
      argv[i] = temp_ptr;
      strncpy(argv[i], "", TOKEN_BUFFER);
      add_history(h, argv[0], exitStatus/255, argv);
    }
  //Pipe command
  } else if(fail_flag == 0) {
    //Initialize individual arrays
    char** argv1 = (char**) malloc(sizeof(char*) * ARGV_BUFFER);
    char** argv2 = (char**) malloc(sizeof(char*) * ARGV_BUFFER);

    //Malloc temp space
    for(int j = 0; j < ARGV_BUFFER; j++) {
      argv1[j] = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
      strncpy(argv1[j], "", TOKEN_BUFFER);
      argv2[j] = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
      strncpy(argv2[j], "", TOKEN_BUFFER);
    }

    //argv1 will take all arguments before the pipe
    for(int j = 0; j < pipe_flag; j++) {
      strncpy(argv1[j], argv[j], TOKEN_BUFFER);
    }
    char* temp_1 = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
    strncpy(temp_1, argv1[pipe_flag], TOKEN_BUFFER);
    free(argv1[pipe_flag]);
    argv1[pipe_flag] = NULL;

    //argv will take all arguments after the pipe
    int k = 0;
    for(int j = pipe_flag + 1; j < i; j++) {
      strncpy(argv2[k], argv[j], TOKEN_BUFFER);
      k++;
    }
    char* temp_2 = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
    strncpy(temp_2, argv2[k], TOKEN_BUFFER);
    free(argv2[k]);
    argv2[k] = NULL;

    //Set up pipe inlet and outlet
    int pipefd[2];
    pipe(pipefd);
    
    //Execute in two child processes
    int pid = fork();
    if(pid == 0) {
      //Child handles argv2
      dup2(pipefd[0], 0);
      close(pipefd[1]);
      int r = execvp(argv2[0], argv2);
      if(r < 0) {
        fprintf(stderr, "ERR\n");
        exit(127);
      } else {
        fprintf(stderr, "P2 complete");
        exit(0);
      }
    } else {
      pid = fork();
      if(pid == 0) {
        //Child handles argv1
        dup2(pipefd[1], 1);
        close(pipefd[0]);
        int r = execvp(argv1[0], argv1);
        if(r < 0) {
          fprintf(stderr, "ERR\n");
          exit(127);
        } else {
          fprintf(stderr, "P1 complete\n");
          exit(0);
        }
      } else {
        wait(NULL);
        printf("Done!\n");
      }
    }

    //Regain NULL ptr
    argv1[pipe_flag] = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
    strncpy(argv1[pipe_flag], temp_1, TOKEN_BUFFER);
    argv2[k] = (char*) malloc(sizeof(char) * TOKEN_BUFFER);
    strncpy(argv2[k], temp_2, TOKEN_BUFFER);

    //Free space
    for(int j = 0; j < ARGV_BUFFER; j++) {
      free(argv1[j]);
      free(argv2[j]);
    }
    free(temp_1);
    free(temp_2);
    free(argv1);
    free(argv2);

  }

  //In case I/O redirection was used, revert to stdin and stdout and close file descriptors
  dup2(s_in, 0);
  dup2(s_out, 1);
  close(s_in);
  close(s_out);
}


//Clears history and exits smash with code 0
void exit_smash() {
  for(int j = 0; j < ARGV_BUFFER; j++) {
    free(argv[j]);
  }
  free(argv);
  clear_history(h);
  exit(0);
}
