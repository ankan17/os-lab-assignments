/* Write a program to create two matrices, A and B, of sizes 40x40, which only
take binary inputs 0 and 1. Create two threads running parallely where one thread
will compute the summation of the two matrices and the other will compute their
difference. There will be one main thread which will wait for the two paralell
threads to complete their task and it will compute the multiplication of the two
previously obtained output and print the final output.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATRIX_SIZE 40

void* sum_runnable(void* input) {
  printf("Summation thread started\n");
  int*** matrices = (int***)input;
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrices[2][i][j] = matrices[0][i][j] + matrices[1][i][j];
    }
  }
  printf("Summation thread done\n\n");
  pthread_exit(NULL);
}

void* diff_runnable(void* input) {
  printf("Difference thread started\n");
  int*** matrices = (int***)input;
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrices[3][i][j] = matrices[0][i][j] - matrices[1][i][j];
    }
  }
  printf("Difference thread done\n\n");
  pthread_exit(NULL);
}

void multiply_matrices(int*** matrices) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrices[4][i][j] = 0;
      for (int k = 0; k < MATRIX_SIZE; k++) {
        matrices[4][i][j] += matrices[2][i][k] * matrices[3][k][j];
      }
    }
  }
}

void print_matrix(int** matrix) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      printf("%3d ", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}


int main (int argc, char *argv[]) {

  /* Check if arguments are correctly provided */
  if (argc != 3) {
    printf("Wrong number of arguments! Please provide the two filenames correctly\n");
    exit(1);
  }

  /* Check if the file is opening properly */
  FILE* fp1 = fopen(argv[1], "r");
  FILE* fp2 = fopen(argv[2], "r");
  if (!fp1) {
    printf("Could not open the file %s\n", argv[1]);
    exit(1);
  } else if (!fp2) {
    printf("Could not open the file %s\n", argv[2]);
    exit(1);
  }

  /* Memory allocation */
  int** matrices[5];
  for (int i = 0; i < 5; i++) {
    matrices[i] = (int**)malloc(MATRIX_SIZE * sizeof(int*));
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrices[i][j] = (int*)malloc(MATRIX_SIZE * sizeof(int));
    }
  }

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      fscanf(fp1, "%d", &matrices[0][i][j]);
      fscanf(fp2, "%d", &matrices[1][i][j]);
    }
  }

  fclose(fp1);
  fclose(fp2);

  printf("Input matrix 1:\n");
  print_matrix(matrices[0]);
  printf("Input matrix 2:\n");
  print_matrix(matrices[1]);

  pthread_t sum_thread, diff_thread;
  pthread_attr_t attr;
  int rc1, rc2;
  void* status;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  rc1 = pthread_create(&sum_thread, &attr, sum_runnable, (void*)matrices);
  rc2 = pthread_create(&diff_thread, &attr, diff_runnable, (void*)matrices);
  if (rc1) {
    printf("ERROR; return code from pthread_create() is %d\n", rc1);
    exit(-1);
  }
  if (rc2) {
    printf("ERROR; return code from pthread_create() is %d\n", rc2);
    exit(-1);
  }

  /* Free attribute and wait for the two threads */
  pthread_attr_destroy(&attr);
  rc1 = pthread_join(sum_thread, &status);
  rc2 = pthread_join(diff_thread, &status);
  if (rc1) {
    printf("ERROR; return code from pthread_join() is %d\n", rc1);
    exit(-1);
  }
  if (rc2) {
    printf("ERROR; return code from pthread_join() is %d\n", rc2);
    exit(-1);
  }

  printf("Main: completed join with summation thread\nSummation matrix:\n");
  print_matrix(matrices[2]);

  printf("Main: completed join with difference thread\nDifference matrix:\n");
  print_matrix(matrices[3]);

  multiply_matrices(matrices);
  printf("Matrix multiplication done\nFinal output:\n");
  print_matrix(matrices[4]);

  printf("Main: program completed. Exiting\n");
  pthread_exit(NULL);
}
