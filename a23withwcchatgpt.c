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

    // Count words using "wc" command
    // Create a pipe for "wc" command's output
    int wc_pipe[2];
    if (pipe(wc_pipe) < 0) {
      perror("Error: Pipe creation failed");
      return 1;
    }

    // Fork the process
    pid_t wc_pid = fork();
    if (wc_pid < 0) {
      perror("Error: Fork failed");
      return 1;
    }

    // Child process: execute "wc" command
    if (wc_pid == 0) {
      // Close unused end of pipe
      close(wc_pipe[0]);

      // Redirect standard input to pipe
      dup2(pipefd[0], STDIN_FILENO);

      // Redirect standard output to pipe
      dup2(wc_pipe[1], STDOUT_FILENO);

      // Execute "wc" command
      execlp("wc", "wc", "-w", NULL);

      // If execlp returns, it means there was an error
      perror("Error: execlp failed");
      exit(1);
    }

    // Parent process: read word count from "wc" command's output
    else {
      // Close unused ends of pipes
      close(pipefd[0]);
      close(wc_pipe[1]);

      // Read word count from "wc" command's output
      char wc_output[256];
      read(wc_pipe[0], wc_output, sizeof(wc_output));
      int word_count = atoi(wc_output);

      // Wait for "wc" command to finish
      wait(NULL);

      printf("Number of words written by child: %d\n", word_count);
      return 0;
    }
  }
}
