//Headers
#include "history.h"

//Builds data structures for recording cmd history
//Returns pointer to history struct
struct history* init_history(int capacity) {
  //Initialize a pointer to history and allocate space for it
  struct history* hp = (struct history*) malloc(sizeof(struct history));
  if(hp == NULL) {
    malloc_error_exit();
  }

  //Initialize the values of the history struct
  hp -> capacity = capacity;
  hp -> offset = 1;
  hp -> entries = (struct cmd**) malloc(sizeof(struct cmd*) * capacity);
  if(hp -> entries == NULL) {
    malloc_error_exit();
  }

  //Allocate space for an initialize the variables of each cmd struct
  for(int i = 0; i < capacity; i++) { 
    //Allocate struct
    hp -> entries[i] = (struct cmd*) malloc(sizeof(struct cmd));
    if(hp -> entries[i] == NULL) {
      malloc_error_exit();
    }

    //Allocate struct pointers
    hp -> entries[i] -> pid = (void*) malloc(sizeof(&(hp -> entries[i])));
    if(hp -> entries[i] -> pid == NULL) {
      malloc_error_exit();
    }
    hp -> entries[i] -> cmd = (char*) malloc(sizeof(char) * CHAR_BUFFER);
    if(hp -> entries[i] -> cmd == NULL) {
      malloc_error_exit();
    }
    hp -> entries[i] -> argv = (char**) malloc(ARGV_BUFFER * sizeof(char*));
    if(hp -> entries[i] -> pid == NULL) {
      malloc_error_exit();
    }
    for(int j = 0; j < ARGV_BUFFER; j++) {
      hp -> entries[i] -> argv[j] = (char*) malloc(sizeof(char) * CHAR_BUFFER);
      if(hp -> entries[i] -> argv[j] == NULL) {
        malloc_error_exit();
      }
      strncpy(hp -> entries[i] -> argv[j], "", CHAR_BUFFER);
    }

    //Initialize
    memcpy((hp -> entries[i] -> pid), (&(hp -> entries[i])), 8);
    strncpy(hp -> entries[i] -> cmd, "", CHAR_BUFFER);
    hp -> entries[i] -> exitStatus = -1;
  }
  
  //Finally, return hp
  return hp;
}

//Adds an entry to the history
void add_history(struct history* h, char* cmd, int exitStatus, char** argv) {
  for(int i = 0; i < h -> capacity; i++) {
    if(h -> entries[i] -> exitStatus == -1) { //This entry is vacant
      //Fill in values for this entry
      strncpy(h -> entries[i] -> cmd, cmd, CHAR_BUFFER);
      h -> entries[i] -> exitStatus = exitStatus;
      for(int j = 0; j < ARGV_BUFFER; j++) {
        strncpy(h -> entries[i] -> argv[j], argv[j], CHAR_BUFFER);
      }
      break;
    } else if (i == h -> capacity - 1) { //The list is full
      //Shift everything back (this removes the first element of the list and creates a vacant spot in the back)
      for(i = 1; i < h -> capacity; i++) {
        strncpy(h -> entries[i - 1] -> cmd, h -> entries[i] -> cmd, CHAR_BUFFER);
        h -> entries[i - 1] -> exitStatus = h -> entries[i] -> exitStatus;
        for(int j = 0; j < ARGV_BUFFER; j++) {
          strncpy(h -> entries[i - 1] -> argv[j], h -> entries[i] -> argv[j], CHAR_BUFFER);
        }
      }
      //Increment the offset
      h -> offset++;
      //Fill in values for this entry
      strncpy(h -> entries[h -> capacity - 1] -> cmd, cmd, CHAR_BUFFER);
      h -> entries[h -> capacity - 1] -> exitStatus = exitStatus;
      for(int j = 0; j < ARGV_BUFFER; j++) {
        strncpy(h -> entries[h -> capacity -1] -> argv[j], argv[j], CHAR_BUFFER);
      }
      break;
    }
  }
}

//Prints the history to stdout
void print_history(struct history* h) {
  //If element exists in entries, print value using a standardized format
  for(int i = 0; i < h -> capacity; i++) {
    if(h -> entries[i] -> exitStatus != -1 /*check if entry is null*/) {
      printf("%d [%d] ", ((h -> offset) + i), h -> entries[i] -> exitStatus);
      //Print contents of argv
      int j = 0;
      while(strncmp(h -> entries[i] -> argv[j], "", CHAR_BUFFER) != 0) {
        printf("%s ", h -> entries[i] -> argv[j]);
        j++;
      }
      printf("\n");
    }
  }
}

//Frees all malloc'd memory in the history
void clear_history(struct history* h) {
  for(int i = 0; i < h -> capacity; i++) {
    for(int j = 0; j < ARGV_BUFFER; j++) {
      free(h -> entries[i] -> argv[j]);
    }
    free(h -> entries[i] -> argv);
    free(h -> entries[i] -> cmd);
    free(h -> entries[i] -> pid);
    free(h -> entries[i]);
  }
  free(h -> entries);
  free(h);
}

void malloc_error_exit() {
  printf("Failed to allocate memory. Exiting program.\n");
  exit(1);
}
