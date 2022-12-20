#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Check if there are enough arguments
  if (argc < 2) {
    fprintf(stderr, "Error: Missing command to execute\n");
    return 1;
  }

  // Create an unnamed pipe
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("Error: Pipe creation failed");
    return 1;
  }

  // Fork the process
  pid_t pid = fork();
  if (pid < 0) {
    perror("Error: Fork failed");
    return 1;
  }

  // Child process: replace with "com" command
  if (pid == 0) {
    // Close unused end of pipe
    close(pipefd[0]);

    // Redirect standard output to pipe
    dup2(pipefd[1], STDOUT_FILENO);

    // Execute "com" command
    execvp(argv[1], &argv[1]);

    // If execvp returns, it means there was an error
    perror("Error: execvp failed");
    exit(1);
  }

  // Parent process: count words written by child
  else {
    // Close unused end of pipe
    close(pipefd[1]);

    // Count words
    int word_count = 0;
    char buffer[256];
    while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
      // Split string into words
      char* word = strtok(buffer, " ");
      while (word != NULL) {
        word_count++;
        word = strtok(NULL, " ");
      }
    }

    // Wait for child to finish
    wait(NULL);

    printf("Number of words written by child: %d\n", word_count);
    return 0;
  }
}
