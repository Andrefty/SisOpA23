#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    fprintf(stderr, "Eroare: com lipseste\n");
    fprintf(stderr, "Utilizare: %s comanda\n",argv[0]);
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
    close(pipefd[1]);
    execvp(argv[1], &argv[1]);

    perror(argv[1]);
    exit(1);
  }

  else
  {

    close(pipefd[1]);
    dup2(pipefd[0],STDIN_FILENO);
    close(pipefd[0]);
    int word_count = 0;
    char buffer[256];
    int bytes_read;
    int in_word = 0; 
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
    { 
      for (int i = 0; i < bytes_read; i++)
      {
        if (isspace(buffer[i]))
        {
          if (in_word)
          { 
            word_count++;
            in_word = 0;
          }
        }
        else
        { 
          in_word = 1;
        }
      }
    }
    if (bytes_read == -1)
    {
      perror("read");
      return 1;
    }
    if (in_word)
    {
      word_count++;
    }
    wait(NULL);

    printf("Numarul de cuvinte scrise de procesul copil: %d\n", word_count);
    return 0;
  }
}
