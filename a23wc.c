#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    fprintf(stderr, "Eroare: com lipseste\n");
    return 1;
  }

  int pipefd[2];
  if (pipe(pipefd) < 0)
  {
    perror("Nu s-a putut crea tubul");
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0)
  {
    perror("Eroare: nu s-a putut face fork");
    return 1;
  }

  if (pid == 0)
  {

    close(pipefd[0]);

    dup2(pipefd[1], STDOUT_FILENO);

    execvp(argv[1], &argv[1]);

    perror("Eroare: executarea com a esuat");
    exit(1);
  }

  else
  {

    close(pipefd[1]);

    int wc_pipe[2];
    if (pipe(wc_pipe) < 0)
    {
      perror("wc:Nu s-a putut crea tubul");
      return 1;
    }

    pid_t wc_pid = fork();
    if (wc_pid < 0)
    {
      perror("wc: nu s-a putut face fork");
      return 1;
    }

    if (wc_pid == 0)
    {

      close(wc_pipe[0]);

      dup2(pipefd[0], STDIN_FILENO);

      dup2(wc_pipe[1], STDOUT_FILENO);

      execlp("wc", "wc", "-w", NULL);

      perror("Eroare: executarea wc -w a esuat");
      exit(1);
    }

    else
    {

      close(pipefd[0]);
      close(wc_pipe[1]);

      char wc_output[256];
      read(wc_pipe[0], wc_output, sizeof(wc_output));
      int word_count = atoi(wc_output);

      wait(NULL);

      printf("Numarul de cuvinte scrise de procesul copil: %d\n", word_count);
      return 0;
    }
  }
}
