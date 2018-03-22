#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREAD  10

int thread_ret[NUM_THREAD];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int range_start;
int range_end;
int result = 0;

bool IsPrime(int n) {
    if (n < 2) {
        return false;
    }

    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

void* ThreadFunc(void* arg) {
    long tid = (long)arg;

        
    while(1) {
        
        pthread_mutex_lock(&mutex);
        printf("%ld thread go sleep\n",tid);
        pthread_cond_wait(&cond, &mutex);  
        
        printf("%ld start workload\n",tid);
        // Split range for this thread
        int start = range_start + ((range_end - range_start) / NUM_THREAD) * tid;
        int end = range_start + ((range_end - range_start) / NUM_THREAD) * (tid+1);
        if (tid == NUM_THREAD - 1) {
            end = range_end + 1;
        }
    
        long cnt_prime = 0;
        for (int i = start; i < end; i++) {
            if (IsPrime(i)) {
                cnt_prime++;
            }
        }
        printf("done work!\n");
        result ++;
        printf("%ld result : %d\n", tid, result);

        thread_ret[tid] = cnt_prime;

        if (result == NUM_THREAD) {
            result = 0;
            printf("wake up main!\n");
            pthread_cond_signal(&cond2);
        }
        pthread_mutex_unlock(&mutex);
    }
        
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREAD];  
    
    // Create threads to work
    for (long i = 0; i < NUM_THREAD; i++) {
        if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
            printf("pthread_create error!\n");
            return 0;
        }
    }
    
    while (1) {
        // Input range
        scanf("%d", &range_start);
        if (range_start == -1) {
            break;
        }
        scanf("%d", &range_end);

        pthread_cond_broadcast(&cond);

        
        pthread_mutex_lock(&mutex);
        printf("go sleep\n");
        pthread_cond_wait(&cond2, &mutex);
        pthread_mutex_unlock(&mutex);

        /*
        // Create threads to work
        for (long i = 0; i < NUM_THREAD; i++) {
            if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
                printf("pthread_create error!\n");
                return 0;
            }
        }

        // Wait threads end
        for (int i = 0; i < NUM_THREAD; i++) {
            pthread_join(threads[i], NULL);
        }
        */

        // Collect results
        int cnt_prime = 0;
        for (int i = 0; i < NUM_THREAD; i++) {
            cnt_prime += thread_ret[i];
        }
        printf("number of prime: %d\n", cnt_prime);
    }
 
    return 0;
}

