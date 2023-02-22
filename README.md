# Project
This is a basic implementation of a shell program written in C. It has a main loop that repeatedly prompts the user for input, parses that input into a shell command, and executes the command.

The program uses several functions to accomplish this. The CommandPrompt() function displays the command prompt, which includes the current working directory, and waits for user input. The user's input is then passed to the ParseCommandLine() function, which parses it into a ShellCommand struct that includes the command arguments and any input/output file redirection information. Finally, the ExecuteCommand() function executes the command specified in the ShellCommand struct.

The ShellCommand struct contains the following fields:

argv: an array of strings representing the command arguments
argc: the number of command arguments
input_file: a string representing the input file to use for the command (or NULL if no input file is specified)
output_file: a string representing the output file to use for the command (or NULL if no output file is specified)
The ParseCommandLine() function uses the strtok() function to tokenize the user's input string, splitting it into separate words based on whitespace. It then processes each token as follows:

If the token is "<", it sets the input_file field of the ShellCommand struct to the next token (which should be the input file name).
If the token is ">", it sets the output_file field of the ShellCommand struct to the next token (which should be the output file name).
Otherwise, it adds the token to the argv array of the ShellCommand struct.
The ExecuteCommand() function uses the fork() system call to create a child process, and then calls execvp() to replace the child process with the specified command. The function also handles input/output file redirection by calling dup2() to redirect standard input/output to the specified files (if any).

Overall, this program provides a basic shell interface that can execute simple commands and handle file redirection. However, it lacks many of the features and capabilities of a full-featured shell, such as command history, job control, and shell scripting.
