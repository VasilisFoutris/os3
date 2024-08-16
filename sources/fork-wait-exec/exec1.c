/* This is just like exec2.c, but it uses "cat" as the pathname instead of "bin/cat". */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{
  char *newargv[3];

  newargv[0] = "cat";
  newargv[1] = "exec1.c";
  newargv[2] = NULL;

  execvp("cat", newargv);
  perror("exec1: execve failed");
  exit(1);
}
