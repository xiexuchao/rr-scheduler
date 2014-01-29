#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRCESSES 100
#define MAX_ARGUMENTS 10

typedef struct context_st {
  unsigned long pid;
  char **args;
  char *command;
  struct proc_context *next;
} proc_context;

extern proc_context* add_proc(char** args, int argc);
extern void run_proc(proc_context* proc);
