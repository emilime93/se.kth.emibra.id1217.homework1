/*
    TASK 2: QUICKSORT
    Author: Emil Lindholm Brandt
    Class: ID1217
*/

/*
    1st argument is problem size
    2nd argument is num workers
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAXSIZE 10000   /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

/* FUNCTION DECLARATIONS */
void quicksort(int *, int, int);
int partition(int *, int, int);
void swap(int *, int *);
void *worker(void *);
double read_timer();

/* GLOBALS */
int problemSize;
int numWorkers;


/* Pthread setup*/
// TODO: Add pthread declarations

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

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void quicksort(int *arr, int lo, int hi) {
    if (lo < hi) {
        int pivot_location = partition(arr, lo, hi);
        quicksort(arr, lo, pivot_location);
        quicksort(arr, pivot_location+1, hi);
    }
}

int partition(int *arr, int lo, int hi) {
    int i;
    int pivot = arr[lo];
    int leftWall = lo;

    for (int i = lo+1; i < hi; i++) {
        if (arr[i] < pivot) {
            swap(&arr[i], &arr[leftWall + 1]);
            leftWall += 1;
        }
    }
    swap(&arr[lo], &arr[leftWall]);

    return leftWall;
}

void *worker(void *arg) {
    long myID = (long)arg;
    printf("My id is: %ld\n", myID);

    int *myAns = calloc(problemSize, sizeof(int));

    int i;
    for (i = 0; i < problemSize; i++) {
        myAns[i] = myID;
    }
    pthread_exit((void *) myAns);
}

int main(int argc, char const *argv[]) {
    long i;

    if (argc > 1) {
        problemSize = atoi(argv[1]);
    }
    problemSize = (problemSize > MAXSIZE) ? MAXSIZE : problemSize;
    if (argc > 2) {
        numWorkers = atoi(argv[2]);
    }
    numWorkers = (numWorkers > MAXWORKERS) ? MAXWORKERS : numWorkers;

    /* MALLOC */
    int *ans = calloc(problemSize, sizeof(int));

    /* THREAD START */

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t workerID[numWorkers];

    double start_time = read_timer();

    // START WORKERS HERE
    for (i = 0; i < numWorkers; i++) {
        pthread_create(&workerID[i], &attr, worker, (void *)i);
    }
    for (i = 0; i < numWorkers; i++) {
        pthread_join(workerID[i], (void *) &ans);
    }

    double end_time = read_timer();

    printf("Time taken: %f\n", end_time-start_time);

    for (i = 0; i < problemSize; i++) {
        printf("Ans is: %d\n", ans[i]);
    }

    free(ans);

    pthread_exit(NULL);
}
