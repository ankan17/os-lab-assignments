/* Write a C program prog.c that creates a tree of processes in order to
recursively compute the maximum in an array of integers. The process at the root
of the tree reads the count n of integers in the array as a command-line parameter.
An array A of size n is then populated with randomly generated integers of small
value (in the range 0-127). The initially unsorted array is printed by the root
process. Any process in the tree handles a chunk of the array A. The chunk is
delimited by two indices L and R. For the root process, L=0 and R=n-1.

Any process P in the tree (including the root) first counts the number of integers
in the chunk it has got. If that count is less than 10, then the process itself
computes the maximum element in its chunk, prints it and exits. If the chunk size
of P is 10 or more, then P creates two child processes PL and PR which handle the
chunks [L, M] and [M+1, R] in A respectively, where M = (L+R)/2. P waits until the
two child processes PL and PR exit. It then computes the maximum of the two maximum
values computed by PL and PR, prints the maximum and exits.

Every non-root process returns to its parent (via the exit status) the maximum
value of its chunk. During the printing of the maximum by a process P, the PID
and the parent PID of P are also printed. The code is expected to handle values
in the range of 50 - 100.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MIN_SIZE 50
#define MAX_SIZE 100

int *A;

int getMax(int l, int r) {
  int max = -1;
  for (int i = l ; i <= r ; ++i)
    max = max > A[i] ? max : A[i];
  return max;
}

int recurFindMax(int l, int r) {
  if (r-l < 10) {
    printf("Current process: %u, Parent process: %u\n", getpid(), getppid());
    return getMax(l, r);
  }

  int pid1, pid2, status1, status2;

  if ((pid1 = fork()) == 0)
    exit(recurFindMax(l, (l+r)/2));
  else {
    if ((pid2 = fork()) == 0)
      exit(recurFindMax((l+r)/2, r));
    else {
      waitpid(pid1, &status1, 0);
      waitpid(pid2, &status2, 0);
      status1 = WEXITSTATUS(status1);
      status2 = WEXITSTATUS(status2);

      printf("Current process: %u, Parent process: %u\n", getpid(), getppid());
      return status1 > status2 ? status1 : status2;
    }
  }
}

int main(int argc, char* argv[]) {
  int n = atoi(argv[1]);
  if (n < MIN_SIZE && n > MAX_SIZE) {
    printf("The value of n should be between %d and %d\n", MIN_SIZE, MAX_SIZE);
    exit(1);
  }
  A = (int*)malloc(n*sizeof(int));
  srand((unsigned int)time(NULL));

  for (int i = 0 ; i < n ; ++i)
    A[i] = rand() % 128;

  printf("A[] = ");
  for (int i = 0 ; i < n ; ++i)
    printf("%d ", A[i]);
  printf("\n");

  int res = recurFindMax(0, n-1);
  printf("The maximum value in the array is %d\n", res);

  return 0;
}
