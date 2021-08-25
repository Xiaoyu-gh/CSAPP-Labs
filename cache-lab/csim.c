// Cache Simulator:
// use trace file as input
// print out counts of hits, misses and evictions

#include "cachelab.h" // required
#include <stdio.h>  // IO
#include <stdlib.h>  // library functions
#include <getopt.h>  // use getopt function
#include <stdbool.h>  // use bool type
#include <stdint.h>  // use uint64_t


// define data structures(begin with capital letter)
struct CacheLine {
    // since it's only for simulation
    // and count hit and miss times
    // "block" section is unneeded
    bool valid;
    uint64_t tag; // unsigned 64-bit int
    int timeStamp; // used for LRU policy
};
typedef struct CacheLine* CacheSet;
typedef CacheSet* Cache; // so Cach is Cacheline**

static int s, E, b; // cache size
static bool verbose = false; // verbose mode flag
FILE *pfile; // pointer to FILE struct, used to read tracefile
Cache cache;

static int currentTime = 0;

// counters, print out as result
static int hits, misses, evictions;

void parseArguments(int, char**);
void cacheInitialize();
void simulate();
int visitCache(uint64_t);
int hitLine(CacheSet, uint64_t);
int updateCache(CacheSet, uint64_t);



int main(int argc, char **argv) {
    parseArguments(argc, argv);
    cacheInitialize();
    simulate();
    printSummary(hits, misses, evictions);
    return 0;
}


// Parse command line options and arguments
// initialize cache size: s, E and b(global variable)
void parseArguments(int argc, char **argv) {
    const char* usage = "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n";
    int opt;

    // least number of command line arguments
    if (argc < 9) {
        printf("Wrong arguments!\n");
        printf(usage, argv[0]);
        exit(1);
    }

    // looping over each argument
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        // determine which argument it's processing
        // optarg is a char*, whitch stored option arguments
        switch(opt) {
            case 'h':
                printf(usage, argv[0]);
                exit(1);
            case 'v':
                verbose = true;
                break;
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
                pfile = fopen(optarg, "r");
                if (pfile == NULL) {
                    printf("Open wrong trace file.\n");
                    exit(1);
                }
                break;
            default:
                printf(usage, argv[0]);
                exit(1);
        }
    }
}

void simulate() {
    char flag; // I, L, S, or M
    uint64_t address; // operation address
    int size; // just for formatting, no actual use

    int visitState; // return value of visitCache

    // loop through all file
    // each iteration process one line
    // Note: white-space in format string matches any amount of space
    // INCLUDE none, in the input, so 'I' will still be read
    while (fscanf(pfile, " %c %lx,%d\n", &flag, &address, &size) == 3) {
        if (flag == 'I') continue;

        switch (flag) {
            case 'L':
                visitState = visitCache(address);
                break;
            case 'S':
                visitState = visitCache(address);
                break;
            case 'M':
                visitState = visitCache(address);
                hits++;
                break;
            default:
                printf("fscanf reads wrong input\n");
        }

        // each operation processed
        currentTime++;
        
        // implement verbose mode
        if (verbose) {
            switch (visitState) {
                case 0:
                    if (flag == 'M') {
                        printf("%c %lx,%d hit hit\n", flag, address, size);
                    }
                    else printf("%c %lx,%d hit\n", flag, address, size);
                    break;
                case 1:
                    if (flag == 'M') {
                        printf("%c %lx,%d miss hit\n", flag, address, size);
                    }
                    else printf("%c %lx,%d miss\n", flag, address, size);
                    break;
                case 2:
                    if (flag == 'M') {
                        printf("%c %lx,%d miss eviction hit\n",
                                flag, address, size);
                    }
                    else printf("%c %lx,%d miss eviction \n",
                                 flag, address, size);
                    break;
                default:
                    printf("visit state wrong\n");
            }
        }

    }

    int S = (1 << s); // 2^s
    for (int i = 0; i < S; i++) {
        free(cache[i]);
    }
    free(cache);
    fclose(pfile);
}

// return value:
// 0 -- hit
// 1 -- miss
// 2 -- miss + eviction
// used to elplement verbose mode
int visitCache(uint64_t address) {
    int t = 64 - s - b;
    uint64_t setIdx = (address << t) >> (t + b);
    uint64_t tag = address >> (s + b);
    CacheSet cacheset = cache[setIdx];

    int line = hitLine(cacheset, tag);

    if (line != -1) {
        hits++;
        // if hit, update timeStamp
        cacheset[line].timeStamp = currentTime;
        return 0;
    }
    else {
        misses++;
        return updateCache(cacheset, tag);
    }

}

// if hit, return the hit line index with that cache set
// else return -1(a miss)
int hitLine(CacheSet set, uint64_t tag) {
    for (int i = 0; i < E; i++) {
        if (set[i].valid && tag == set[i].tag) {
            return i;
        }
    }
    return -1;
}

// return value:
// 1 -- miss
// 2 -- miss + eviction
int updateCache(CacheSet set, uint64_t tag) {
    int lru = currentTime; // least recent used time

    // use least recently used replacment policy
    for (int i = 0; i < E; i++) {
        // if empty, store in it
        if (!set[i].valid) {
            set[i].valid = true;
            set[i].tag = tag;
            set[i].timeStamp = currentTime;
            return 1;
        }
        // find the lru time
        if (set[i].timeStamp < lru) {
            lru = set[i].timeStamp;
        }
    }
    
    // find the lru one and evict it
    // having min timeStamp means lru
    for (int i = 0; i < E; i++) {
        if (set[i].timeStamp == lru) {
            set[i].tag = tag;
            set[i].timeStamp = currentTime;
            evictions++;
            return 2;
        }
    }
    
    return -1; // error
} 



// allocate space for cache and initialize it with 0
void cacheInitialize() {
    int S = (1 << s); // number of sets

    // allocate memory for cache
    // CacheSet is pointer, whose size is 8
    cache = (Cache)malloc(sizeof(CacheSet) * S);
    if (cache == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // allocate memory for each CacheSet in cache
    for (int i = 0; i < S; i++) {
        // must use calloc but not malloc, to initialize with 0
        cache[i] = (CacheSet)calloc(E, sizeof(struct CacheLine));
        if (cache[i] == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
    }
}
