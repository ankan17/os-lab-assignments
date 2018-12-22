# OS Lab Assignments

This repository contains the solutions to the Operating Systems Lab Assignments
given to CSE 5th semester students of 2018-19 batch.


### Assignment 1

__Topics:__ fork() system call

Write a C program prog.c that creates a tree of processes in order to
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

__Files:__ [prog.c](https://github.com/ankan17/os-lab-assignments/blob/master/prog.c)


### Assignment 2

__Topics:__ exec() system call

Write a collection of programs p1, p2 and p3 such that they execute sequentially
with the same PID and each program should also print its PID. The user should be
able to invoke any combination of these programs to achieve the required
functionality. For example - Consider three programs twice, half and square
which accept only one integer as argument and does some specific operation.
These operations may be like -
  - $twice 10 prints 20 and some number which is its PID
  - $half 10 prints 5 and some number which is its PID
  - $square 10 prints 100 and some number which is its PID

Now the user should be able to combine these programs in any combination to
achieve the desired result. For example -
  - Input: $twice square half twice half 10
    Output: [half[twice[half[square[twice 10]]]]]
  - Input: $square twice 10
    Output: [twice[square 10]]

__Files:__ [half.c](https://github.com/ankan17/os-lab-assignments/blob/master/half.c) [twice.c](https://github.com/ankan17/os-lab-assignments/blob/master/twice.c) [square.c](https://github.com/ankan17/os-lab-assignments/blob/master/square.c)


### Assignment 3

__Topics:__ Interprocess communication through pipe, signal handling

Write a C program to implement the following game.

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

__Files:__ [childsgame.c](https://github.com/ankan17/os-lab-assignments/blob/master/childsgame.c)


### Assignment 4

__Topics:__ Different system calls

In this assignment, you start writing a command interpreter (shell).

The shell will give a prompt for the user to type in a command from the set of
commands. Take the command, execute it and give the prompt back to the user for
the next command. Your program should be able to do the following:

  1. Give a prompt "myshell>" for the user to type in a command
  2. Implement the following built in commands:
      a. `cd <dir>`     :   Changes the directory to dir
      b. `pwd`          :   Prints the current directory
      c. `mkdir <dir>`  :   Creates the directory dir
      d. `rmdir <dir>`  :   Removes the directory dir
      e. `ls`           :   Lists the files and directories in the present directory
      f. `exit`         :   Exits out of the shell
  3. Any other command typed at the prompt should be executed as if it is the
  name of the executable file. For example - typing "a.out" should execute the
  file a.out. This file can be in the current directory or any of the other
  directories specified by PATH environment variable.
  4. The executable file should be executed after creating a new process and
  executing the file onto it. The parent process should wait for the file to
  finish execution and then go on to read the next command from the user. The
  executable can take any number of command line parameters.

Name this file as myshell.c.

__Files:__ [myshell.c](https://github.com/ankan17/os-lab-assignments/blob/master/myshell.c)


### Assignment 5

__Topics:__ Semaphore, shared memory

Consider a file containing a maximum of 100 student records. The record for each
student is kept in one line in the file and contains the following, separated by
one or more spaces: First_name (ascii string, one word with no spaces in between,
max. 20 characters), Last_Name (ascii string, one word with no spaces in between,
max 20 characters), Roll_No (integer), CGPA (float). Roll no. is unique for a
student, but others may be the same for two students.

A process X, when started, first loads all the records in shared memory from a
file whose name is passed to it as a command line argument. It then goes to
sleep, waking up periodically (say every 5 seconds) to check if any of the data
have been modified (how will it know that?). If yes, it writes the entire content
of the shared memory back to the file. The process X runs forever.

Any number of other processes can query and update the data by reading/writing the
shared memory. Specifically, a querying process Y is allowed to do the following
operations on the data:
  - Search for a student record by roll no. (should show all data for that student)
  - Update the CGPA of a particular student (the student is identified by the
  roll no., which is unique for each student)
  - Exit

The process Y will keep on doing the above two in a loop until the exit option
is chosen. Note that any number of copies of Y can be started at one time
(think of this as multiple people searching/updating student records at the same
time). Also, the following criterions must be ensured:
  - If any copy of Y is started before X is started, it should wait for X to be
  started first.
  - Only X should create and initialize all necessary shared memory.

Write two C programs, X.c for the process X and Y.c for the process Y. Your
programs must be well-commented to make it easier for anyone to understand what
semaphores you are using and how they are used. Submit the files X.c and Y.c.

__Files:__ X.c, Y.c


### Assignment 6

__Topics:__ Threads

Write a program to create two matrices, A and B, of sizes 40x40, which only
take binary inputs 0 and 1. Create two threads running parallelly where one thread
will compute the summation of the two matrices and the other will compute their
difference. There will be one main thread which will wait for the two parallel
threads to complete their task and it will compute the multiplication of the two
previously obtained output and print the final output.

__Files:__ [pthread.c](https://github.com/ankan17/os-lab-assignments/blob/master/pthread.c)
