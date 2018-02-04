/*
    TASK 1: Matrix Sum, min & max.
    Author: Emil Lindholm Brandt
    Class: ID1217
*/
/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#define MAXSIZE 10000   /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t lock;   /* Lock for  */
int numWorkers;          /* number of workers */
int numArrived = 0;      /* number who have arrived */

typedef struct {
    long value;
    long i;
    long j;
} Index;

/* timer */
double read_timer()
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized)
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time;  /* start and end times */
int size, stripSize;          /* assume size is multiple of numWorkers */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

/*  */
Index minIndex;
Index maxIndex;
long sum = 0;

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[])
{
    int i, j;
    long l; /* use long in case of a 64-bit system */
    minIndex.value = LONG_MAX;
    maxIndex.value = LONG_MIN;

    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* initialize mutex and condition variable */
    pthread_mutex_init(&lock, NULL);

    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE)
        size = MAXSIZE;
    if (numWorkers > MAXWORKERS)
        numWorkers = MAXWORKERS;
    stripSize = size / numWorkers;

    printf("===== RUN INFO =====\n");
    printf("Problem Size:\t%d\nNum Workers:\t%d\n\n", size, numWorkers);

    srand(time(NULL));

    /* initialize the matrix */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = rand() % 99; // 1;
        }
    }

    /* print the matrix */
#ifdef DEBUG
    for (i = 0; i < size; i++)
    {
        printf("[ ");
        for (j = 0; j < size; j++)
        {
            printf(" %d", matrix[i][j]);
        }
        printf(" ]\n");
    }
#endif

    /* Read timer for start time */
    start_time = read_timer();

    /* do the parallel work: create the workers */
    for (l = 0; l < numWorkers; l++) {
        pthread_create(&workerid[l], &attr, Worker, (void *)l);
    }
    /* Wait for all threads to complete their work */
    for (l = 0; l < numWorkers; l++) {
        pthread_join(workerid[l], NULL);
    }

    /* get end time */
    end_time = read_timer();

    /* print results */
    printf("The execution time is %g sec\n", end_time - start_time);
    printf("Total: %ld\nMax: %ld (%ld, %ld)\nMin: %ld (%ld, %ld)\n", sum, maxIndex.value, maxIndex.i, maxIndex.j, minIndex.value, minIndex.i, minIndex.j);

    pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg)
{
    long myid = (long)arg;
    int total, i, j, first, last;
    Index min_index, max_index;
    min_index.value = LONG_MAX;
    max_index.value = LONG_MIN;

    /* determine first and last rows of my strip */
    first = myid * stripSize;
    last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

    /* sum values in my strip */
    total = 0;
    for (i = first; i <= last; i++) {
        for (j = 0; j < size; j++) {
            total += matrix[i][j];
            if (matrix[i][j] > max_index.value) {
                max_index.value = matrix[i][j];
                max_index.i = i;
                max_index.j = j;
            }
            if (matrix[i][j] < min_index.value) {
                min_index.value = matrix[i][j];
                min_index.i = i;
                min_index.j = j;
            }
        }
    }

    pthread_mutex_lock(&lock);
    sum += total;
    if (max_index.value > maxIndex.value) {
        maxIndex.value = max_index.value;
        maxIndex.i = max_index.i;
        maxIndex.j = max_index.j;
    }
    if (min_index.value < minIndex.value) {
        minIndex.value = min_index.value;
        minIndex.i = min_index.i;
        minIndex.j = min_index.j;
    }
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}
