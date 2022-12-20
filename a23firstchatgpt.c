#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: nrc com [arguments]\n");
        return 1;
    }

    // Create an unnamed pipe
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("Failed to create pipe");
        return 1;
    }

    // Fork the process
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Failed to fork process");
        return 1;
    }

    if (pid == 0)
    {
        // This is the child process
        close(pipefd[0]);   // Close the read end of the pipe
        dup2(pipefd[1], 1); // Replace stdout with the write end of the pipe
        close(pipefd[1]);   // Close the write end of the pipe
        // Execute the command
        char *com_argv[argc];
        com_argv[0] = argv[1]; // Set the first argument to the command name
        for (int i = 2; i < argc; i++)
        {
            // Set the remaining arguments to the command arguments
            com_argv[i - 1] = argv[i];
        }
        com_argv[argc - 1] = NULL; // Set the last argument to NULL
        execvp(com_argv[0], com_argv);
        perror("Failed to execute command");
        return 1;
    }
    else
    {
        // This is the parent process
        close(pipefd[1]); // Close the write end of the pipe
        int word_count = 0;
        char c;
        while (read(pipefd[0], &c, 1) > 0)
        {
            // Read one character at a time from the pipe
            if (c == ' ' || c == '\n')
            {
                word_count++; // Increment the word count if we encounter a space or newline
            }
        }
        close(pipefd[0]); // Close the read end of the pipe
        // Execute the command
        char *com_argv[argc];
        com_argv[0] = argv[1]; // Set the first argument to the command name
        for (int i = 2; i < argc; i++)
        {
            // Set the remaining arguments to the command arguments
            com_argv[i - 1] = argv[i];
        }
        com_argv[argc - 1] = NULL; // Set the last argument to NULL
        execvp(com_argv[0], com_argv);
        perror("Failed to execute command");
        return 1;
    }
    else
    {
        // This is the parent process
        close(pipefd[1]); // Close the write end of the pipe
        int word_count = 0;
        char c;
        while (read(pipefd[0], &c, 1) > 0)
        {
            // Read one character at a time from the pipe
            if (c == ' ' || c == '\n')
            {
                word_count++; // Increment the word count if we encounter a space or newline
            }
        }
        close(pipefd[0]); // Close the read end of the pipe
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        printf("Number of words written by the command: %d\n", word_count);
        return 0;
    }
}