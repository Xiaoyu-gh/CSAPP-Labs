#ifndef _CACHE_H_
#define _CACHE_H_

#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define NCACHELINE 10
 
typedef struct cacheline {
    char *name;
    char *data;
    int datasize;
    int last_used_time;
    pthread_rwlock_t rwlock; 
} cacheline_t;

typedef cacheline_t* cache_t;

extern int global_time;

void cache_init(cache_t *cache);
void cache_free(cache_t *cache);
int is_hit(cache_t cache, char *uri, int *idx);
void cache_serve(cache_t cache, int idx, int connfd);
void write_to_cache(cache_t cache, char *uri, char *object, int size);

#endif
