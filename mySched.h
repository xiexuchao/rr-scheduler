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

struct context_st {
  pid_t pid;
  char **args;
  char *command;
  struct context_st *next;
  struct context_st *prev;
};

extern struct itimerval intervalTimer;
extern struct sigaction alarmSig;
extern int curr_pid;

extern void new_itimer(int quantum);
extern struct context_st* init_proc(char** args);
extern struct context_st* remove_proc(struct context_st *first_proc);
extern void run_all_procs(struct context_st *first_proc, int quantum);
extern void alarm_sig_catch(int signum);
extern void fork_all_procs(struct context_st *first_proc);

#endif
