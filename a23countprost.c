#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  
  if (argc < 2) {
    fprintf(stderr, "Eroare: com lipseste\n");
    return 1;
  }

  
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("Nu s-a putut crea tubul");
    return 1;
  }

  
  pid_t pid = fork();
  if (pid < 0) {
    perror("Eroare: nu s-a putut face fork");
    return 1;
  }

  
  if (pid == 0) {
    
    close(pipefd[0]);

    
    dup2(pipefd[1], STDOUT_FILENO);

    
    execvp(argv[1], &argv[1]);

    
    perror("Eroare: executarea com a esuat ");
    exit(1);
  }

  
  else {
    
    close(pipefd[1]);

    
    int word_count = 0;
    char buffer[256];
    while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
      
      char* word = strtok(buffer, " ");
      while (word != NULL) {
        word_count++;
        word = strtok(NULL, " ");
      }
    }

    
    wait(NULL);

    printf("Numarul de cuvinte scrise de procesul copil: %d\n", word_count);
    return 0;
  }
}
