#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define MAX_PRCESSES 100
#define MAX_ARGUMENTS 10

typedef struct context_st {
  unsigned long pid;
  char **args;
  char *command;
  struct proc_context *next;
} proc_context;

extern struct itimerval new_itimer(int quantum);
extern proc_context* add_proc(char** args, int arg_counter);
extern void run_proc(proc_context* proc, int quantum);
extern char* append_path(char *arg);
extern void run_all_procs(proc_context *curr_proc, int quantum);
