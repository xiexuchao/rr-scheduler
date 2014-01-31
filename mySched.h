#ifndef MY_SCHED_H
#define MY_SCHED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#define MAX_PROCESSES 100
#define MAX_ARGUMENTS 10
#define SEC_TO_USEC 1000
#define RE 0
#define WE 1

struct context_st {
  pid_t pid;
  char **args;
  char *command;
  struct context_st *next;
  struct context_st *prev;
};

extern void new_itimer(int quantum);
extern struct context_st* init_proc(char** args);
extern struct context_st* remove_proc(struct context_st *first_proc);
extern void run_proc(struct context_st* proc);
extern char* append_path(char *arg);
extern void run_all_procs(struct context_st *first_proc, int quantum);

#endif
