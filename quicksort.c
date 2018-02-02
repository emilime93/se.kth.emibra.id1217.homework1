/*
    TASK 2: QUICKSORT
    Author: Emil Lindholm Brandt
    Class: ID1217
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define MAXSIZE = 10000
#define MAXWORKERS = 10;

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

int main(int argc, char const *argv[]) {

    int numWorkers, workSize;

    if (argc > 1) {
        workSize = (int)argv[1];
    }
    workSize = (workSize > MAXSIZE) ? MAXSIZE : workSize;
    if (argc > 2) {
        numWorkers = (int)argv[2];
    }
    numWorkers = (numWorkers > MAXWORKERS) ? MAXWORKERS : workSize;

    pthread_attr_t attr;

    double start_time = read_timer();

    // START WORKERS HERE

    double end_time = read_timer();

    printf("Time taken: %f\n", end_time-start_time);
    return 0;
}
