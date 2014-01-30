#include "mySched.h"

void run_proc(proc_context* proc, int quantum) {
  int status;
  pid_t child;

  child = fork();

  if (child) {
    waitpid(child, &status, 0);
  } else {
    execvp(proc->command, proc->args);
    exit(-1);
  }
}

proc_context* add_proc(char** args, int arg_counter) {
  args[arg_counter] = NULL;
  proc_context *proc = malloc(sizeof(proc_context));
  proc->command = args[0];
  proc->args = args;
  return proc;
}

char* append_path(char *arg) {
  int str_len = strlen(arg) + strlen("./");
  char *str = malloc(sizeof(char) * str_len);
  strcpy(str, "./");
  strcat(str, arg);
  return str;
}

void run_all_procs(proc_context *curr_proc, int quantum) {
  while(curr_proc) {
    run_proc(curr_proc, quantum);
    curr_proc = curr_proc->next;
  }
}

int main(int argc, char *argv[]) {
  // 0 is schedule, 1 is quantum, 2 starts args
  int quantum = strtol(argv[1], NULL, 0);
  int index = 2;
  char *args[MAX_ARGUMENTS];
  int arg_counter = 0;
  proc_context* last_proc = NULL;
  proc_context* curr_proc = NULL;
  proc_context* first_proc = NULL;

  while (index < argc) {
    if (strcmp(argv[index], ":") == 0) {
      curr_proc = add_proc(args, arg_counter);
      if (last_proc != NULL) {
        last_proc->next = curr_proc;
      }
      if (first_proc == NULL) {
        first_proc = curr_proc;
        run_proc(first_proc, quantum);
      }
      last_proc = curr_proc;
      while (arg_counter > 0) {
        args[arg_counter] = NULL;
        arg_counter--;
      }
    } else {
      if (arg_counter == 0) {
        args[arg_counter] = append_path(argv[index]);
      } else {
        args[arg_counter] = argv[index];
      }
      arg_counter++;
    }
    index++;
  }
  curr_proc = add_proc(args, arg_counter);

  run_all_procs(first_proc, quantum);
}
