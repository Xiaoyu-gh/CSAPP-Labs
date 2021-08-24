#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

// global variable
static int s, E, b;
FILE *pfile; // pointer to FILE struct, used to read tracefile

int main(int argc, char **argv) {
    parseArguments(argc, argv);


    struct cacheLine {
        // since it's only for simulation
        // and count hit and miss times
        // "block" section is unneeded
        bool valid;
        unsigned tag;
        int useCounter;
    };
    
    // dynamically allocate memory for cache
    // which is a 2D array of cacheline
    struct cacheLine **cache = malloc(sizeof(cacheLine));

}

// Phase1: parse command line options and arguments
// initialize cache size: s, E and b
void parseArugument(int argc, char **argv) {
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
                break;
            default:
                printf("Wrong argument\n");
                break;
        }
    }
}












printSummary(0, 0, 0);
    return 0;

