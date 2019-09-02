//Include
#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

//vars
#ifndef _HIS_ALOC
#define _HIS_ALOC
struct history* h;

//Exe comm prototype
void executeCommand(char* str);
//void sigint_handler();
#endif
