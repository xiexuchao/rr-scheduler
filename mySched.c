#include "mySched.h"

void run_proc(proc_context* proc) {
  int status;
  pid_t child;

  child = fork();

  if (child) {
    waitpid(child, &status, 0);
  } else {
    //printf("%s %s \n", proc->command, proc->args[1]);
    execvp(proc->command, proc->args);
    //char *arr[] = {"ls", "-a", NULL};
    //execvp("ls", arr);
    exit(-1);
  }
}

proc_context* add_proc(char** args, int argc) {
  proc_context *proc = malloc(sizeof(proc_context));
  proc->command = args[0];
  proc->args = args;
  return proc;
}

int main(int argc, char *argv[]) {
  unsigned long quantum = strtol(argv[1], NULL, 0);
  int index = 2; // 0 is schedule and 1 is quantum
  char *args[MAX_ARGUMENTS];
  int num_procs = 0;
  int arg_counter = 0;
  proc_context* last_proc;
  proc_context* curr_proc;

  while (index < argc) {
    if (strcmp(argv[index], ":") == 0) {
      args[arg_counter] = NULL;
      curr_proc = add_proc(args, arg_counter);
      run_proc(curr_proc);
      if(last_proc) {
        //last_proc->next = curr_proc;
      }
      last_proc = curr_proc;
      while (arg_counter > 0) {
        args[arg_counter] = NULL;
        arg_counter--;
      }
      num_procs++;
    } else {
      args[arg_counter] = argv[index];
      arg_counter++;
    }
    index++;
  }
  num_procs++;
  args[arg_counter] = NULL;
  curr_proc = add_proc(args, arg_counter);
  run_proc(curr_proc);
  fflush(stdout);
}
