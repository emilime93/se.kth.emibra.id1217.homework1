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
void *quicksort(void *);
int partition(int *, int, int);
void swap(int *, int *);
double read_timer();
void printArray(int *);
void seq_quicksort(int *, int, int);

/* GLOBALS */
long problemSize;
int numWorkers;
pthread_t workerID[MAXWORKERS];

/* Struct for organizing arguments */
typedef struct {
    long id;
    int *arr;
    int lo;
    int hi;
} Work_Args;


/* Pthread setup*/

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

void printArray(int *arr) {
    long i;
    printf("[ ");
    for (i = 0; i < problemSize; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void seq_quicksort(int *arr, int lo, int hi) {
    if (lo < hi) {
        int pivot_location = partition(arr, lo, hi);
        seq_quicksort(arr, lo, pivot_location);
        seq_quicksort(arr, pivot_location+1, hi);
    }
}

void *quicksort(void *arg) {
    Work_Args *args = (Work_Args *) arg;
    long myID = args->id;
    int* arr = args->arr;
    int lo = args->lo;
    int hi = args->hi;

    int pivot_location = partition(arr, lo, hi);

    if (myID+2 > MAXWORKERS) {
        seq_quicksort(arr, lo, pivot_location);
        seq_quicksort(arr, pivot_location+1, hi);
    } else if (lo < hi) {

        // Make attributes for threads
        pthread_attr_t attr1;
        pthread_attr_init(&attr1);
        pthread_attr_t attr2;
        pthread_attr_init(&attr2);

        /* Create the arguments to pass along to new workers */
        Work_Args args1 = (Work_Args) {.id = myID+2, .arr = arr, .lo = lo, .hi = pivot_location};
        Work_Args args2 = (Work_Args) {.id = myID+1, .arr = arr, .lo = pivot_location+1, .hi = hi};

        pthread_create(&workerID[myID+2], &attr1, quicksort, (void *) &args1);
        pthread_join(workerID[myID+2], (void *) &arr);
        quicksort(&args2);
        
    }
    pthread_exit((void *) arr);
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

int main(int argc, char const *argv[]) {
    long i;

    /* INITIALIZE THE SIZES */
    problemSize = (argc > 1) ? problemSize = atoi(argv[1]) : MAXSIZE;
    problemSize = (problemSize > MAXSIZE) ? MAXSIZE : problemSize;

    // WORKERS -1 due to using heap structure and therefore skipping index 0
    numWorkers = (argc > 2) ? numWorkers = atoi(argv[2]) : MAXWORKERS ;
    numWorkers = (numWorkers >= MAXWORKERS) ? MAXWORKERS: numWorkers;

    printf("===== RUN INFO =====\n");
    printf("Problem Size:\t%ld\nNum Workers:\t%d\n\n", problemSize, numWorkers);

    /* MALLOC space for answer */
    int *ans = calloc(problemSize, sizeof(int));

    /* THREAD START */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    /* TEST DATA */

    srand(time(NULL));
    for (i = 0; i < problemSize; i++) {
        ans[i] = rand() % 100 +1;
    }

    #ifdef DEBUG
        printf("Unsorted array:\n");
        printArray(ans);
    #endif

    /* Create the arguments for the first worker */
    Work_Args args = (Work_Args) {.id = 1, .arr = ans, .lo = 0, .hi = problemSize};


    /* Start the timers, and the workers */
    double start_time = read_timer();

    pthread_create(&workerID[0], &attr, quicksort, (void *) &args);
    pthread_join(workerID[0], (void *) &ans);

    double end_time = read_timer();

    #ifdef DEBUG
        printf("Sorted Array:\n");
        printArray(ans);
    #endif

    printf("Time taken: %f\n", end_time-start_time);

    free(ans);

    pthread_exit(NULL);
}
