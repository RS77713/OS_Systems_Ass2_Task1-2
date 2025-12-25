// threads.c - Thread synchronization demo using POSIX threads.
// Shows a race condition on a shared counter, then fixes it with a mutex.
// Run with:
//   ./threads

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define ITERATIONS  1000000L  // increments per thread

// Shared resource
static long long counter = 0;

// Mutex to protect the shared counter
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id;
    int use_mutex;
} ThreadArgs;

static void *worker(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int id = args->id;
    int use_mutex = args->use_mutex;

    unsigned long tid = (unsigned long)pthread_self();

    printf("[Thread %d] Started. pthread_t = %lu, use_mutex = %d\n",
           id, tid, use_mutex);
    fflush(stdout);

    for (long long i = 0; i < ITERATIONS; i++) {
        if (use_mutex) {
            pthread_mutex_lock(&counter_mutex);
        }

        // Critical section: increment shared counter
        counter++;

        if (use_mutex) {
            pthread_mutex_unlock(&counter_mutex);
        }

        // Print progress a few times (to avoid spamming)
        if (i == 0 || i == ITERATIONS / 2 || i == ITERATIONS - 1) {
            printf("[Thread %d] Progress: i = %lld, current counter = %lld\n",
                   id, i, counter);
            fflush(stdout);
        }
    }

    printf("[Thread %d] Finished.\n", id);
    fflush(stdout);
    return NULL;
}

static void run_phase(int use_mutex) {
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    counter = 0;
    printf("\n=== Phase (use_mutex = %d) starting ===\n", use_mutex);
    fflush(stdout);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i;
        args[i].use_mutex = use_mutex;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    long long expected = NUM_THREADS * ITERATIONS;
    printf("=== Phase (use_mutex = %d) finished ===\n", use_mutex);
    printf("Expected counter value: %lld\n", expected);
    printf("Actual   counter value: %lld\n", counter);
    if (use_mutex) {
        printf("Result is correct (race condition fixed).\n");
    } else {
        printf("Result is usually wrong because of the race condition.\n");
    }
    fflush(stdout);
}

int main(void) {
    printf("Thread synchronization demo: %d threads, %ld increments each.\n",
           NUM_THREADS, ITERATIONS);

    // Phase 1: no mutex -> race condition
    run_phase(0);

    // Phase 2: with mutex -> correct result
    run_phase(1);

    pthread_mutex_destroy(&counter_mutex);
    return 0;
}
