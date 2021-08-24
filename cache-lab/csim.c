// Cache Simulator:
// use trace file as input
// print out counts of hits, misses and evictions

#include "cachelab.h" // required
#include <stdio.h>  // IO
#include <stdlib.h>  // library functions
#include <unistd.h>  // dependency of getopt.h
#include <getopt.h>  // use getopt function
#include <math.h>  // use calculation like pow()


// define data structures(begin with capital letter)
struct CacheLine {
    // since it's only for simulation
    // and count hit and miss times
    // "block" section is unneeded
    bool valid;
    uint64_t tag; // unsigned 64-bit int
    int useCounter;
};
typedef CacheLine* CacheSet;
typedef CachSet* Cache; // so Cach is Cacheline**

// global variable
static int s, E, b;
FILE *pfile; // pointer to FILE struct, used to read tracefile
Cache cache;

int main(int argc, char **argv) {
    parseArguments(argc, argv);
    cacheInitialize();
    

    printSummary();
    return 0;
}

// Phase1: parse command line options and arguments
// initialize cache size: s, E and b(global variable)
void parseAruguments(int argc, char **argv) {
    int opt;
    // looping over each argument
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        // determine which argument it's processing
        // optarg is a char*, whitch stored option arguments
        switch(opt) {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                // open tracefile,return pfile point to it
                pfile = fopen(optarg, 'r');
                if (pfile == NULL) {
                    printf("Open wrong trace file.\n");
                    exit(1);
                }
                break;
            default:
                printf("Wrong argument\n");
                break;
        }
    }
}

void simulate() {
    cacheInitialze();

    char flag;
    uint64_t address;
    int size; // just for formatting, no actual use

    // loop through all file
    // each iteration process one line
    // Note: fscanf will ignore space while reads stream
    while (fscanf(pfile, "%c %x,%d", &flag, &address, &size) == 3) {
        if (flag == 'I') continue; // skip instruction
        switch (flag) {
            case 'L':
        }

    }
}

// allocate space for cache and initialize it with 0
void cacheInitialize() {
    int S = pow(2, s); // number of sets

    // allocate memory for cache
    // CacheSet is pointer, whose size is 8
    cache = (cache)malloc(sizeof(CacheSet) * S);
    if (cache == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // allocate memory for each CacheSet in cache
    for (int i = 0; i < S; i++) {
        // must use calloc but not malloc, to initialize with 0
        cache[i] = (CacheSet)calloc(E, sizeof(CacheLine));
        if (cache[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
    }
}
