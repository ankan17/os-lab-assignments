/* In this assignment, you start writing a command interpreter (shell).

The shell will give a prompt for the user to type in a command from the set of
commands. Take the command, execute it and give the prompt back to the user for
the next command. Your program should be able to do the following:

  1. Give a prompt "myshell>" for the user to type in a command
  2. Implement the following built in commands:
    a) cd <dir>     :   Changes the directory to dir
    b) pwd          :   Prints the current directory
    c) mkdir <dir>  :   Creates the directory dir
    d) rmdir <dir>  :   Removes the directory dir
    e) ls           :   Lists the files and directories in the present directory
    f) exit         :   Exits out of the shell
  3. Any other command typed at the prompt should be executed as if it is the
  name of the executable file. For example - typing "a.out" should execute the
  file a.out. This file can be in the current directory or any of the other
  directories specified by PATH environment variable.
  4. The executable file should be executed after creating a new process and
  executing the file onto it. The parent process should wait for the file to
  finish execution and then go on to read the next command from the user. The
  executable can take any number of command line parameters.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PROMPT "myshell>"
#define BUFFERSIZE 150

char** split_by_space(char* string) {
  char** tokens = (char**)malloc(sizeof(char*) * 40);
  int i = 0;
  char *token = strtok(string, " ");
  while (token != NULL) {
    tokens[i++] = token;
    token = strtok(NULL, " ");
  }
  tokens[i] = NULL;
  return tokens;
}

int main() {
  char command[BUFFERSIZE];
  int status;
  pid_t pid;

  while (1) {

    // Give the prompt
    printf("%s ", PROMPT);

    // Read the command
    fgets(command, BUFFERSIZE, stdin);
    command[strlen(command) - 1] = '\0';

    // Exit out of the program if the command is "exit"
    if (strcmp(command, "exit") == 0)
      break;

    // If the command is "cd", use chdir() system call in the parent process
    else if (strstr(command, "cd") != NULL) {
      char dirname[BUFFERSIZE/3];
      strcpy(dirname, command + 3);
      int ret = chdir(dirname);
      if (ret)
        printf("%s: No such directory\n", dirname);
    }

    // If the command is "cd", use getcwd() system call in the parent process
    else if (strcmp(command, "pwd") == 0) {
      char cwd[BUFFERSIZE];
      getcwd(cwd, BUFFERSIZE);
      printf("%s\n", cwd);
    }

    // If the command is "mkdir", use mkdir() system call and give 755 permission
    else if (strstr(command, "mkdir") != NULL) {
      char dirname[BUFFERSIZE/3];
      strcpy(dirname, command + 6);
      int status = mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      if (status)
        printf("%s: Directory could not be created due to some error!\n", dirname);
    }

    // If the command is "rmdir", use rmdir() system call
    else if (strstr(command, "rmdir") != NULL) {
      char dirname[BUFFERSIZE/3];
      strcpy(dirname, command + 6);
      int status = rmdir(dirname);
      if (status)
        printf("%s: Directory could not be removed due to some error!\n", dirname);
    }

    // If the command is ls, use readdir() system call and filter out entries like
    // ., .. and any hidden files beginning with . to match the format used by ls
    else if (strcmp(command, "ls") == 0) {
      struct dirent *dp;
      DIR *dirp = opendir(".");
      int count = 0;
      if (dirp != NULL) {
        while ((dp = readdir(dirp)) != NULL) {
          if (dp->d_name[0] != '.') {
            if (++count % 4)
              printf("%-16s", dp->d_name);
            else
              printf("%-16s\n", dp->d_name);
          }
        }
        closedir(dirp);
        if (count % 4 != 0)
          printf("\n");
      } else {
        printf("Could not get the files of the current directory\n");
      }
    }

    // For any other command, spawn a child process and execute the process in it
    else {
      if ((pid = fork()) == 0) {
        char **tokens = split_by_space(command);
        int file_in_cur_dir = 0;

        // Check if the file is present in the current directory
        struct dirent *dp;
        DIR *dirp = opendir(".");
        if (dirp != NULL) {
          while ((dp = readdir(dirp)) != NULL) {
            if (strcmp(tokens[0], dp->d_name) == 0) {
              file_in_cur_dir = 1;
              break;
            }
          }
          closedir(dirp);
        }

        // If the file is present in the current directory, add the current
        // directory to the PATH. This only lasts in the child process.
        if (file_in_cur_dir) {
          const char* PATH = "PATH";
          char* path = getenv(PATH);

          char newpath[BUFFERSIZE*10];
          getcwd(newpath, BUFFERSIZE);  // New path starts with the current directory
          strcat(newpath, ":");
          strcat(newpath, path);        // Concatenate ":" followed by old path

          setenv(PATH, newpath, 1);     // Set the PATH environment variable
        }
        execvp(tokens[0], tokens);
      }
      else {
        waitpid(pid, &status, 0);
      }
    }
  }
}
