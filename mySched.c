#include "mySched.h"

struct itimerval intervalTimer;
struct sigaction alarmSig;
int pid;
int quantum;

// creates proc context
struct context_st* init_proc(char** args) {
  //args[arg_counter] = NULL;
  struct context_st *proc = malloc(sizeof(struct context_st));
  proc->args = args;
  proc->command = proc->args[0];
  proc->pid = -1;
  return proc;
}

// sets path string for command for executables
char* append_path(char *arg) {
  int str_len = strlen(arg) + strlen("./");
  char *str = malloc(sizeof(char) * str_len);
  strcpy(str, "./");
  strcat(str, arg);
  return str;
}

// Creates all the child processes, sets up piping for debugging output
// And immediately stops the children as they're created.
void fork_all_procs(struct context_st *curr_proc) {
  int child;

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
  kill(pid, SIGSTOP);
}

// remove finished proc from linked list and free
struct context_st* remove_proc(struct context_st *curr_proc) {
  struct context_st *temp = curr_proc;
  if (curr_proc->prev == curr_proc) {
    free(temp);
    temp = NULL;
    return temp;
  } else {
    curr_proc = (temp->prev)->next = temp->next;
    (temp->next)->prev = temp->prev;
  }
  free(temp);
  return curr_proc;
}

// Function to run the round robin.
void run_all_procs(struct context_st *curr_proc) {
  int status;
  pid_t my_pid;

  memset(&alarmSig, 0, sizeof (alarmSig));
  alarmSig.sa_handler = &alarm_sig_catch;
  sigaction(SIGALRM, &alarmSig, NULL);

  while(curr_proc) {
    pid = curr_proc->pid;
    kill(curr_proc->pid, SIGCONT);
    new_itimer();
    setitimer(ITIMER_REAL, &intervalTimer, NULL);
    my_pid = waitpid(curr_proc->pid, &status, WUNTRACED);

    if (my_pid > 0 && (WIFEXITED(status) || WIFSIGNALED(status))) {
      curr_proc = remove_proc(curr_proc);
    } else {
      curr_proc = curr_proc->next;
    }
  }
}

// sets up the timer
void new_itimer() {
  intervalTimer.it_interval.tv_sec = 0;
  intervalTimer.it_interval.tv_usec = 0;
  intervalTimer.it_value.tv_sec = quantum / 1000;
  intervalTimer.it_value.tv_usec = (quantum % SEC_TO_USEC) * SEC_TO_USEC;
}

// parses input and starts schedule
int main(int argc, char *argv[]) {
  // 0 is schedule, 1 is quantum, 2 starts args
  quantum = strtol(argv[1], NULL, 0);
  int index = 2;
  char *args[MAX_ARGUMENTS];
  int arg_counter = 0;
  struct context_st *curr_proc = NULL;
  struct context_st *first_proc = NULL;

  new_itimer(quantum);
  argv = argv + 2;
  int parsingArgs = 0;
  while (*argv) {
    if (!parsingArgs) {
      parsingArgs = 1;
      struct context_st *last_proc = curr_proc;
      curr_proc = init_proc(argv);
      if (first_proc == NULL) {
        first_proc = curr_proc->next = curr_proc->prev = curr_proc;
      } else {
        last_proc->next = curr_proc;
        curr_proc->next = first_proc;
        curr_proc->prev = last_proc;
        first_proc->prev = curr_proc;
      }
    } else {
      if (**argv == ':') {
        *argv = NULL;
        parsingArgs = 0;
      }
      argv++;
    }
  }

  fork_all_procs(first_proc);
  run_all_procs(first_proc);
}
