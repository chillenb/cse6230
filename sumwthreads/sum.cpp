#include <stdio.h>
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


#define THREAD_LIMIT 100
#define ARRAY_SIZE_LIMIT 1 << 30
#define MAX_ITERS 1 << 31 
#define MULTITHREADING_THRESHOLD 400000

void usage() {
    fprintf(stderr, "usage: sum <N> <P> <T>\n");
    fprintf(stderr, "N: array size; P: max no. of threads; T: no. of iterations\n");
    exit(EXIT_FAILURE);
}

float single_thread_sum(span<float> array);
float sum_with_threads(span<float> array, int p);

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
    span<float> array(buf, n);

    /* Fill the array with pseudorandom floats */
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-20.0, 20.0);
    for(auto it = array.begin(); it != array.end(); it++) {
        *it = dist(gen);
    }
    
    /* First run to warm things up */

    float tot = 0;
    for(int64_t i = 0; i < 10; i++) {
        tot = sum_with_threads(array, p);
    }

    auto start = system_clock::now();

    for(int64_t i = 0; i < t; i++) {
        tot = sum_with_threads(array, p);
    }
    auto stop = system_clock::now();
    
    auto duration = duration_cast<microseconds>(stop-start);
    printf("%ld, %d, %lE\n", n, p, duration.count()/1000000.0 / t);
    free(buf);
    return 0;
}

float sum_with_threads(span<float> array, int p) {
    size_t arrsize = array.size();
    if(p>1 && array.size() > MULTITHREADING_THRESHOLD) {
        /* Skip this part if single-threaded */
        size_t floats_per_thread = arrsize / p;
        size_t remainder = arrsize - p * floats_per_thread;

        std::vector<std::future<float>> VF;
        for(int i = 0; i < p - 1; i++) {
            VF.push_back(std::async(single_thread_sum,
                array.subspan(i * floats_per_thread, floats_per_thread)));
        }
        float remainder_sum = single_thread_sum(array.last(remainder + floats_per_thread));
        float total = 0;
        for(auto& f : VF) {
            total += f.get();
        }
        return total + remainder_sum;
    } else {
        return single_thread_sum(array);
    }
}

/* Very simple, but harder to optimize better than the compiler */
float single_thread_sum(span<float> array) {
    float sum = 0;
    for(float x : array) {
        sum += x;
    }
    return sum;
}