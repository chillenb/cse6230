#include <stdio.h>
#include <stdlib.h>

#define THREAD_LIMIT 100
#define ARRAY_SIZE_LIMIT 1 << 30
#define MAX_ITERS 1 << 14

void usage() {
    fprintf(stderr, "usage: sum <N> <P> <T>\n");
    fprintf(stderr, "N: array size; P: max no. of threads; T: no. of iterations\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv) {
    /* Allow very large array sizes */
    int64_t n; int p; int t;
    if (argc != 4) {
        usage();
    }

    /* Get arguments */
    n = strtol(argv[1], NULL, 10);
    p = atoi(argv[2]);
    t = atoi(argv[3]);

    if (!(n > 0 && p > 0 && t > 0)) {
        usage();
    } else if (p > THREAD_LIMIT || t > MAX_ITERS) {
        usage();
    }

    float* array;
    array = malloc(n * sizeof(float));
    if(array == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    

    free(array);
    return 0;
}
