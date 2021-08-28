#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
/*
#include <thread>
#include <span>
#include <random>
#include <future>
#include <chrono>

using std::thread;
using std::span;
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
*/

#define THREAD_LIMIT 100
#define ARRAY_SIZE_LIMIT 1 << 30
#define MAX_ITERS 1 << 31 
#define MULTITHREADING_THRESHOLD 1000000

void usage() {
    fprintf(stderr, "usage: sum <N> <P> <T>\n");
    fprintf(stderr, "N: array size; P: max no. of threads; T: no. of iterations\n");
    exit(EXIT_FAILURE);
}

typedef struct {
    float* buf;
    int64_t size;
    float* returnval;
} chunk;

void * single_thread_sum(void*);
float simple_sum(float*, int64_t);
float multi_thread_sum(float*, int64_t, int p);

int main(int argc, char** argv) {
    /* Allow very large array sizes */
    int64_t n; int p; int64_t t;
    if (argc != 4) {
        usage();
    }

    /* Get arguments */
    n = strtol(argv[1], NULL, 10);
    p = atoi(argv[2]);
    t = strtol(argv[3], NULL, 10);

    if (!(n > 0 && p > 0 && t > 0)) {
        usage();
    } else if (p > THREAD_LIMIT) {
        usage();
    }
    
    /* Allocate memory for the array */
    float* buf;
    buf = (float*) malloc(n * sizeof(float));
    if(buf == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    //span<float> array(buf, n);

    /* Fill the array with pseudorandom floats */
    /*
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-20.0, 20.0);
    for(auto it = array.begin(); it != array.end(); it++) {
        *it = dist(gen);
    }
    */

    srand((unsigned int)time(NULL));
    for(int64_t i = 0; i < n; i++) {
        *(buf+i) = ((float)rand()/(float)(RAND_MAX)) * 40.0 - 20.0;
    }

    /* First run to warm things up */
    //multi_thread_sum(array,p);
    long start_sec, end_sec, start_usec, end_usec;
    struct timeval timecheck;

    gettimeofday(&timecheck, NULL);

    start_sec = (long) timecheck.tv_sec;
    start_usec = (long) timecheck.tv_usec;


    //auto start = system_clock::now();
    float tot = 0;
    float recent = 0;
    for(int64_t i = 0; i < t; i++) {
        recent = multi_thread_sum(buf, n, p);
        tot += recent;
    }
    
    /*
    auto stop = system_clock::now();
    
    auto duration = duration_cast<microseconds>(stop-start);
    printf("%ld, %d, %lE\n", n, p, duration.count()/1000000.0 / t);
    */

    gettimeofday(&timecheck, NULL);
    end_sec = (long) timecheck.tv_sec;
    end_usec = (long) timecheck.tv_usec;
    double diff_time = (double) (end_sec - start_sec) + (double) (end_usec - start_usec) / 1000000.0;

    printf("%ld, %d, %lE\n", n, p, diff_time / t);
    printf("Simple sum gives %lf\n", simple_sum(buf, n));
    printf("Difference: %lf\n", recent - simple_sum(buf, n));

    //printf("%f\n", tot - single_thread_sum(array));
    free(buf);
    return 0;
}

float multi_thread_sum(float* array, int64_t n, int p) {
    if(p>1){// && array.size() > MULTITHREADING_THRESHOLD) {
        /* Skip this part if single-threaded */
        size_t floats_per_thread = n / p;
        size_t remainder = n - p * floats_per_thread;

        chunk* paramvector = (chunk*) malloc(p*sizeof(chunk));
        float* returnvector = (float*) malloc(p*sizeof(float));
        pthread_t* threadvector = (pthread_t*) malloc((p-1)* sizeof(pthread_t));


        for(int i = 0; i < p - 1; i++) {
            (paramvector+i)->buf = array + i*floats_per_thread;
            (paramvector+i)->size = floats_per_thread;
            (paramvector+i)->returnval = returnvector+i;
            pthread_create(threadvector+i, NULL, &single_thread_sum, paramvector+i);
            
            //VF.push_back(std::async(single_thread_sum, array.subspan(i * floats_per_thread, floats_per_thread)));
        }
        chunk remainder_params;
        remainder_params.buf = array + n - remainder;
        remainder_params.size = remainder;
        remainder_params.returnval = returnvector + (p-1);
        //single_thread_sum((void*) &remainder_params);
        //float remainder_sum = single_thread_sum(array.last(remainder));
        float total = simple_sum(array+n-remainder-floats_per_thread, remainder + floats_per_thread);
        //printf("Remainder size %ld\n", remainder + floats_per_thread);
        /*
        for(auto& f : VF) {
            total += f.get();
        }
        */

        for(int i = 0; i < p - 1; i++) {
            pthread_join(*(threadvector+i), NULL);
            total += *(returnvector+i);
        }

        free(paramvector); free(returnvector);
        return total;
    } else {
        return simple_sum(array, n);
    }
}

float simple_sum(float* array, int64_t size) {
    float total = 0.0;
    for(int64_t i = 0; i < size; i++) {
        total += *(array+i);
    }
    return total;
}

/* Very simple, but harder to optimize better than the compiler */
void * single_thread_sum(void* v) {
    chunk* params = (chunk*) v;
    float sum = 0;
    for(int64_t i = 0; i < params->size; i++) {
        sum += *(params->buf+i);
    }
    //printf("%f\n", sum);
    //printf("Chunk size %ld\n", params->size);
    *(params->returnval) = sum;
}