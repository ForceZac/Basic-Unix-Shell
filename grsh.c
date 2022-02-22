#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define DELIMITERS " \t\v\n\r"

void exit_grsh(char **args);
void cd(char **aargs);
void set_path(char **args);


//Globals
const char *builtin_cmds[3] = {"exit","cd","path"}; // array of strings.
void (*builtin_cmd_functs[]) (char **) = {&exit_grsh, &cd, &set_path}; // array of pointers to functions.
const char error_message[30] = "An error has occurred\n";
char path[16384] = "/bin"; // Default path to executables.
char *cmd[20] = {[0 ... 19] = NULL}; // command to be executed.
int num_args = 0;

// A debugging fn used to print a string array.
void print_string_array(char **arr) {
  static int calls = 0; // number of times the function gets called.
  calls++;
  for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) {
    printf("cmd array, call %d: [%s]\n", calls, arr[i]);
  }
}

// A fn that is used to exit the shell.
void exit_grsh(char **args) {
  exit(0);
}

// A fn that is used to change directories.
void cd(char **args) {
  char *cd_path = args[1];
  if (cd_path == NULL)
  {
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
  if (chdir(cd_path) != 0) {
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
}

// A fn that is used to overwrite the existing path.
void set_path(char **args) {
  if (args[1] == NULL) { // If no arguments are passed, set path to be empty.
    memset(path, 0, sizeof(path)); //path = "";
  }
  else {
    memset(path, 0, sizeof(path)); //path = ""; // Reset the path to overwrite.
    strcpy(path, args[1]);
    int i = 2;
    while (args[i] != NULL) { // Paths are separated by colons (:).
      strcat(path, ":");
      strcat(path, args[i]);
      ++i;
    }
  }
}

// This function will launch a new process
int launcher() {
  pid_t pid;
  char exec_path[4096]; // Path to the executable file.
  memset(exec_path, 0, sizeof(exec_path));
  char *save_ptr;
  char *path_token;

  path_token = strtok_r(strdup(path), ":", &save_ptr);
  while(path_token != NULL) {
    strcpy(exec_path, path_token);
    strcat(exec_path, "/");
    strcat(exec_path, cmd[0]);
    if (access(exec_path, X_OK) == 0) {
      break;
    }

    memset(exec_path, 0, sizeof(exec_path)); // exec_path = "";
    path_token = strtok_r(NULL, ":", &save_ptr);
  }

  if (strcmp(exec_path, "") == 0) {
    write(STDERR_FILENO, error_message, strlen(error_message));
    return 1;
  }

  pid = fork();
  if (pid < 0) {
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(EXIT_FAILURE);
  }
  else if (pid == 0) {
    execv(exec_path, cmd); // Execute the command.
    // If execv returns, we know the command failed to execute.
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(EXIT_FAILURE);
  }
  else {
    int status;
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

// Function to execute a single command from user.
int execute_cmd() {
  if (num_args == 0) { // No command was passed.

    return 0;
}

  // Checks to see if the given
  // command, is one of the builtin commands: path, cd or exit.
  for (int i = 0; i < sizeof(builtin_cmds)/sizeof(builtin_cmds[0]); ++i) {
    if (strcmp(cmd[0], builtin_cmds[i]) == 0) {
      (*builtin_cmd_functs[i])(cmd);

      return 1;
    }
  }

  return launcher();
}

// This function executes multiple commands at a time.
int execute_shell(char *input) {
  int exec_status = 0; // Checks the "yay" or "nay" status.  0 = False, 1 = True.
  int output, error, save_output, save_error;
  char *save_ptr;
  char *token;

  token = strtok_r(strdup(input), DELIMITERS, &save_ptr);
  while (token != NULL) {
    if (strcmp(token, "&") == 0) { // To execute a parallel command from user.

      exec_status = execute_cmd();
    }
    else if (strcmp(token, ">") == 0) {

      token = strtok_r(NULL, DELIMITERS, &save_ptr); // Filename.

      // Open output file. (fcntl.h)
      output = open(token, O_RDWR|O_CREAT|O_APPEND, 0600);

      if (output == -1) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
      }

      // Open error file. (fcntl.h)
      error = open(token, O_RDWR|O_CREAT|O_APPEND, 0600);
      if (error == -1) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
      }

      // Save copies of outputs and errors.
      save_output = dup(fileno(stdout));
      save_error = dup(fileno(stderr));

      // Redirect the outputs and errors to stdout and stderr.
      if (dup2(output, fileno(stdout)) == -1) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
      }
      if (dup2(error, fileno(stdout)) == -1) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(EXIT_FAILURE);
      }

      exec_status = execute_cmd();

      // Flush the streams and close them.
      fflush(stdout); close(output);
      fflush(stderr); close(error);

      // Reset to the original file descriptors.
      dup2(save_output, fileno(stdout));
      dup2(save_error, fileno(stderr));

      // Close the file descriptor copies.
      close(save_output);
      close(save_error);
    }
    //Build commands
    else {
      cmd[num_args++] = token;
    }
    // "token" declaration.
    token = strtok_r(NULL, DELIMITERS, &save_ptr); // Advance to next token.

    // Cleanup after command execution.
    if (exec_status) {
      exec_status = 0; // Reset booleon operands
      memset(cmd, 0, sizeof(cmd)); // Clear command arguments.
      num_args = 0;
    }
  }

  // Execute the last command.
  if (num_args > 0) {
    exec_status = execute_cmd();
  }

  // Cleanup after command execution.
  if (exec_status) {
    exec_status = 0; // Reset bool.
    memset(cmd, 0, sizeof(cmd)); // Clear command arguments.
    num_args = 0;
  }

  return 0;
}

// The grsh command interface setup
void grsh_syst() {
  size_t len = 0;      // Size of the users input line.
  ssize_t read;        // Reads status of the input line.
  char *line = NULL;   // The users real-time input line.

  printf("grsh> ");
  while ((read = getline(&line, &len, stdin)) != -1) {
    execute_shell(line);
    printf("grsh> ");
  }
}


void batch_filer(char *filename) {
  size_t len = 0;      // Length of the input line.
  ssize_t read;        // Read status of the input line.
  char *line = NULL;   // The input line.
  FILE *fptr = fopen(filename, "r"); // Open file in read mode.

  if (!fptr) {
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
  }

  while((read = getline(&line, &len, fptr)) != -1) {
    execute_shell(line);
  }

  fclose(fptr);
}

/* Main c Function */
int main(int argc, char *argv[]) { 
  if (argc < 2) {
    grsh_syst();
  }
  else {
    batch_filer(argv[1]);
  }

  return 0;
} //EOF
