/* Write a C program to implement the following game.

The parent program P first creates two pipes and then spawns two child processes
C and D. One of the two pipes is meant for communication between P and C and the
other one for communication between P and D. Now a loop runs as follows.

In each iteration (also called a round), P first randomly chooses one of the
flags MIN or MAX. The choices randomly varies from one iteration to another.
Each of the two child processes C and D generates a random positive integer and
sends that to P via its pipe. P reads the two integers. Let these be c and d
respectively. If P has chosen MIN, then the child which sends the smaller of c
and d gets one point and if P has chosen MAX, then the child which sends the
larger of c and d gets one point. If c == d, then this round is ignored. The
child process which first obtains 10 points wins the game.

When the game ends, P sends a user-defined signal to both C and D and the child
processes exit after handling the signal (in order to know who was the winner).
After C and D exits, the parent process also exists. During each iteration of
the game, P should also print appropriate messages like P's choice of flag,
the integers received from C and D, which child gets the point, the current
scores of C and D in order to let the user know how the game is going on.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

#define BUFSIZE 20
#define WINNING_CONDITION 10

static int stop = 0;

// Custom signal handler to handle the termination of game
void end_process(int sig) {
  if (sig == SIGUSR1) {
    stop = 1;
  }
  else if (sig == SIGUSR2) {
    stop = 2;
  }
}

int main() {
  pid_t pid1, pid2;

  // Create the pipe descriptors
  int fd1[2], fd2[2];
  pipe(fd1);
  pipe(fd2);

  // Assign the custom signal handler to SIGUSR1 signal
  signal(SIGUSR1, end_process);
  signal(SIGUSR2, end_process);

  if ((pid1 = fork()) == 0) {
    // Child 1
    srand((unsigned int)time(NULL) ^ getpid());
    close(fd1[0]);  // Close the read end as the child will not read

    // Generate a random number and place it on the pipe
    while(!stop) {
      int n = rand() % 100;
      char line[BUFSIZE];
      sprintf(line, "%d", n);
      write(fd1[1], line, BUFSIZE);
    }

    // Handle the exit of the child process by displaying proper message
    if (stop == 1) {
      printf("C: I am the winner\n");
    } else if (stop == 2) {
      printf("C: D is the winner\n");
    }
    printf("Exiting from child C\n\n");

  } else {
    if ((pid2 = fork()) == 0) {
      // Child 2
      srand((unsigned int)time(NULL) ^ getpid());
      close(fd2[0]);  // Close the read end as the child will not read

      // Generate a random number and place it on the pipe
      while(!stop) {
        int n = rand() % 100;
        char line[BUFSIZE];
        sprintf(line, "%d", n);
        write(fd2[1], line, BUFSIZE);
      }

      // Handle the exit of the child process by displaying proper message
      if (stop == 1) {
        printf("D: C is the winner\n");
      } else if (stop == 2) {
        printf("D: I am the winner\n");
      }
      printf("Exiting from child D\n\n");

    } else {
      // Parent
      close(fd1[1]);
      close(fd2[1]);  // Close the write ends as the parent will not read

      srand((unsigned int)time(NULL) ^ getpid());
      int round = 1;
      int score1 = 0, score2 = 0;
      char line[BUFSIZE];
      char winner;
      int num1, num2, choice;

      while (1) {
        printf("Round number: %d\n", round++);

        choice = rand() % 2;  // P chooses a flag: 0 for MAX and 1 for MIN

        // Read and print the first integer from the pipe
        read(fd1[0], line, BUFSIZE);
        sscanf(line, "%d", &num1);
        printf("Integer received from C: %d\n", num1);

        // Read and print the second integer from the pipe
        read(fd2[0], line, BUFSIZE);
        sscanf(line, "%d", &num2);
        printf("Integer received from D: %d\n", num2);

        // Choose the winner for the round and add to its point
        if (choice) {
          printf("P's choice of flag: MIN\n");
        } else {
          printf("P's choice of flag: MAX\n");
        }
        if (num1 == num2)
          printf("This round is a tie!\n");
        else if ((num1 < num2 && choice) || (num1 > num2 && !choice)) {
          printf("C gets the point\n");
          score1++;
        } else {
          printf("D gets the point\n");
          score2++;
        }
        printf("Updated scores: C = %d, D = %d\n\n", score1, score2);

        // Break when score of any child becomes 10
        if (score1 == WINNING_CONDITION || score2 == WINNING_CONDITION)
          break;
      }

      // Send appropriate singal to the children to let them know who the winner was
      if (score1 > score2) {
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR1);
      } else {
        kill(pid1, SIGUSR2);
        kill(pid2, SIGUSR2);
      }

      // Wait for the children to terminate and then terminate the parent
      waitpid(pid1, &num1, 0);
      waitpid(pid2, &num2, 0);
      printf("Exiting from parent P\n");
    }
  }
  return 0;
}
