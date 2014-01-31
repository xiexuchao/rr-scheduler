#include "mySched.h"

struct itimerval intervalTimer;
struct sigaction alarmSig;
int curr_pid;

// creates proc context
struct context_st* init_proc(char** args) {
  struct context_st *proc = malloc(sizeof(struct context_st));
  proc->args = args;
  proc->command = proc->args[0];
  proc->pid = -1;
  return proc;
}

// Creates all the child processes, sets up piping for debugging output
// And immediately stops the children as they're created.
void fork_all_procs(struct context_st *first_proc) {
  int child;
  struct context_st *curr_proc = first_proc;

  while(curr_proc->pid == -1) {
    if (child = fork()) {
      curr_proc->pid = child;
    } else {
      raise(SIGSTOP);
      execvp(curr_proc->command, curr_proc->args);
      exit(1);
    }
    curr_proc = curr_proc->next;
  }
}

// Catches sig and stops proc
void alarm_sig_catch(int signum) {
  kill(curr_pid, SIGSTOP);
}

// remove finished proc from linked list and free
struct context_st* remove_proc(struct context_st *curr_proc) {
  struct context_st *temp = curr_proc;
  if (curr_proc->prev == curr_proc || curr_proc->next == curr_proc) {
    free(temp);
    return NULL;
  }
  curr_proc = temp->next;
  (temp->prev)->next = temp->next;
  (temp->next)->prev = temp->prev;
  free(temp);
  return curr_proc;
}

// Function to run the round robin.
void run_all_procs(struct context_st *first_proc, int quantum) {
  int status;
  pid_t my_pid;
  struct context_st *curr_proc = first_proc;

  memset(&alarmSig, 0, sizeof (alarmSig));
  alarmSig.sa_handler = &alarm_sig_catch;
  sigaction(SIGALRM, &alarmSig, NULL);

  while(curr_proc) {
    curr_pid = curr_proc->pid;
    new_itimer(quantum);
    kill(curr_proc->pid, SIGCONT);
    setitimer(ITIMER_REAL, &intervalTimer, NULL);
    my_pid = waitpid(curr_proc->pid, &status, WUNTRACED);

    if (my_pid > 0 && (WIFEXITED(status))
      && !(WSTOPSIG(status)) && !(WIFSTOPPED(status))) {
      curr_proc = remove_proc(curr_proc);
    } else {
      curr_proc = curr_proc->next;
    }
  }
}

// sets up the timer
void new_itimer(int quantum) {
  intervalTimer.it_interval.tv_sec = 0;
  intervalTimer.it_interval.tv_usec = 0;
  intervalTimer.it_value.tv_sec = quantum / 1000;
  intervalTimer.it_value.tv_usec = (quantum % SEC_TO_USEC) * SEC_TO_USEC;
}

// parses input and starts schedule
int main(int argc, char *argv[]) {
  // 0 is schedule, 1 is quantum, 2 starts args
  int quantum = strtol(argv[1], NULL, 0);
  struct context_st *curr_proc = NULL;
  struct context_st *first_proc = NULL;
  struct context_st *last_proc = NULL;
  int new_proc = 0, num_proc = 0, num_args = 0;
  char **curr_arg = argv + 2;

  while (curr_arg[0] != NULL) {
    if (!new_proc) {
      new_proc = 1;
      last_proc = curr_proc;
      curr_proc = init_proc(curr_arg);
      num_proc++;
      if (first_proc == NULL) {
        first_proc = curr_proc;
        curr_proc->next = curr_proc;
        curr_proc->prev = curr_proc;
      } else {
        last_proc->next = curr_proc;
        curr_proc->next = first_proc;
        curr_proc->prev = last_proc;
        first_proc->prev = curr_proc;
      }
    } else {
      if (num_proc > MAX_PROCESSES) {
        fputs("There were too many processes entered.\n", stderr);
        return EXIT_FAILURE;
      }
      else if(num_args > MAX_ARGUMENTS) {
        fputs("There were too many arguments entered.\n", stderr);
        return EXIT_FAILURE;
      }
      else {
        if (!strcmp(curr_arg[0], ":")) {
          curr_arg[0] = NULL;
          new_proc = 0;
          num_args = 0;
        }
      }
      curr_arg++;
      num_args++;
    }
  }

  fork_all_procs(first_proc);
  run_all_procs(first_proc, quantum);
}
