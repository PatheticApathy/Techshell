#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 64
#define MAX_ARG_LEN 256
#define MAX_CMD_LEN 512
#define PROMPT_MAX_LEN 256

// Struct to hold command information
struct ShellCommand{
    char* argv[MAX_ARGS]; // Command arguments
    int argc; // Number of arguments
    char* input_file; // Input file name (or NULL)
    char* output_file; // Output file name (or NULL)
}

// Display the command prompt, which includes the current working directory.
char* CommandPrompt(){
    static char prompt[PROMPT_MAX_LEN];
    char cwd[PROMPT_MAX_LEN];
    getcwd(cwd, sizeof(cwd));
    sprintf(prompt, "%s$ ", cwd);
    printf("%s", prompt);
    return fgets(prompt, sizeof(prompt), stdin);
}

// Parse the user input as a shell command.
struct ShellCommand ParseCommandLine(char* input){
    struct ShellCommand command = { { NULL }, 0, NULL, NULL };
    char* arg;
    char* token = strtok(input, " \t\n");

    while (token != NULL) 
    {
        if (strcmp(token, "<") == 0) 
        {
            token = strtok(NULL, " \t\n");

            if (token != NULL) 
            {
                command.input_file = strdup(token);
                token = strtok(NULL, " \t\n");
            }
            else 
            {
                fprintf(stderr, "Error: missing input file name\n");
                command.argv[0] = strdup("error");
                return command;
            }
        } 
        else if (strcmp(token, ">") == 0) 
        {
            token = strtok(NULL, " \t\n");

            if (token != NULL) 
            {
                command.output_file = strdup(token);
                token = strtok(NULL, " \t\n");
            } 
            else 
            {
                fprintf(stderr, "Error: missing output file name\n");
                command.argv[0] = strdup("error");
                return command;
            }
        } 
        else 
        {
            arg = strdup(token);
            command.argv[command.argc++] = arg;

            if (command.argc >= MAX_ARGS) 
            {
                break;
            }
            token = strtok(NULL, " \t\n");
        }
    }
    return command;
}

// Execute a shell command.
void ExecuteCommand(struct ShellCommand command) {
    pid_t pid = fork();

    if (pid == -1) 
    {
        fprintf(stderr, "Error: fork failed\n");
        exit(1);
    } 
    else if (pid == 0) 
    {
    // Child process
        if (command.input_file != NULL) 
        {
            int fd = open(command.input_file, O_RDONLY);

            if (fd == -1) 
            {
                fprintf(stderr, "Error: cannot open input file '%s': %s\n", command.input_file, strerror(errno));
                exit(1);
            }
            if (dup2(fd, STDIN_FILENO) == -1) 
            {
                fprintf(stderr, "Error: cannot redirect input to '%s': %s\n", command.input_file, strerror(errno));
                exit(1);
            }

            close(fd);
        }
        if (command.output_file != NULL) 
        {
            int fd = open(command.output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            if (fd == -1)
            {
                fprintf(stderr, "Error: cannot open output file '%s': %s\n", command.output_file, strerror(errno));
                exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) == -1) 
            {
                fprintf(stderr, "Error: cannot redirect output to '%s': %s\n", command.output_file, strerror(errno));
                exit(1);
            }

            close(fd);
        }
        if (execvp(command.argv[0], command.argv) == -1) 
        {
            fprintf(stderr, "Error: command '%s' not found\n", command.argv[0]);
            exit(1);
        }
    } 
    else 
    {
        // Parent process
        int status;

        if (waitpid(pid, &status, 0) == -1) 
        {
            fprintf(stderr, "Error: waitpid failed\n");
            exit(1);
        }
        if (WIFEXITED(status)) 
        {
            int exit_status = WEXITSTATUS(status);

            if (exit_status != 0) 
            {
                fprintf(stderr, "Command '%s' failed with exit code %d\n", command.argv[0], exit_status);
            }
        }
    }
}

// Main shell loop
int main(int argc, char* argv[]) {
    char input[MAX_CMD_LEN];

    while (1) 
    {
        if (CommandPrompt() == NULL) 
        {
            break;
        }

        struct ShellCommand command = ParseCommandLine(input);

        if (command.argv[0] == NULL) 
        {
            continue;
        }
        if (strcmp(command.argv[0], "exit") == 0) 
        {
            break;
        }

        ExecuteCommand(command);

        for (int i = 0; i < command.argc; i++) 
        {
            free(command.argv[i]);
        }
        
        free(command.input_file);
        free(command.output_file);
    }
    return 0;
}