/*
 * implement a simple proxy cache based on reader-writer model
 */

#include "csapp.h"
#include "cache.h"

/*
 * init cache with NCACHELINE cache line, each with MAX_OBJECT_SIZE
 * Note: agument must be cache_t * type, but not cache_t
 */
void cache_init(cache_t *cache) {
    *cache = (cache_t)Malloc(sizeof(cacheline_t) * NCACHELINE);
    for (int i = 0; i < NCACHELINE; i++) {
        (*cache)[i].name = (char *)Malloc(sizeof(char) * MAXLINE);
        (*cache)[i].data = (char *)Malloc(sizeof(char) * MAX_OBJECT_SIZE);
        (*cache)[i].datasize = 0;
        (*cache)[i].last_used_time = 0;
        pthread_rwlock_init(&(*cache)[i].rwlock, NULL);
    }
}

/*
 * free allocated memory
 */
void cache_free(cache_t *cache) {
    for (int i = 0; i < NCACHELINE; i++) {
        free((*cache)[i].name);
        free((*cache)[i].data);
        pthread_rwlock_destroy(&(*cache)[i].rwlock);
    }
    free(*cache);
}
/*
 * return 1 if this object is in cache
 * and set cacheline_idx to correspond value
 */
int is_hit(cache_t cache, char *uri, int *idx) {
    int cmp_result;
    for (int i = 0; i < NCACHELINE; i++) {
        if (pthread_rwlock_rdlock(&cache[i].rwlock) != 0)
            unix_error("rdlock");
        cmp_result = strcmp(cache[i].name, uri);
        pthread_rwlock_unlock(&cache[i].rwlock);

        if (!cmp_result) { 
            *idx = i;
            return 1;
        }
    } 
    return 0;
}

/* retrieve object from cache then send back to client */
void cache_serve(cache_t cache, int idx, int connfd) {
    printf("A cache hit!\n");
    
    // TRY to update last_used_time if no other writer holds the lock
    // otherwise, just skip this step
    // so it's not a strict LRU policy but similar 
    // and it support many reader reads concurrently
    if (!pthread_rwlock_trywrlock(&cache[idx].rwlock)) {
        cache[idx].last_used_time = global_time;
        pthread_rwlock_unlock(&cache[idx].rwlock);
    }

    // send back to client
    pthread_rwlock_rdlock(&cache[idx].rwlock);
    Rio_writen(connfd, cache[idx].data, cache[idx].datasize);
    pthread_rwlock_unlock(&cache[idx].rwlock);
    printf("cache_serve success\n\n");
}

void write_to_cache(cache_t cache, char *uri, char *object, int size) {
    int has_empty = 0;
    int victim_idx;

    // find an empty line
    for (int i = 0; i < NCACHELINE; i++) {
        pthread_rwlock_rdlock(&cache[i].rwlock);
        if (cache[i].last_used_time == 0) {
            has_empty = 1;
            victim_idx = i;
            pthread_rwlock_unlock(&cache[i].rwlock);
            break;
        }
        pthread_rwlock_unlock(&cache[i].rwlock);
    }

    // if no empty line, find a LRU line as victim
    int lru = global_time;
    if (!has_empty) {
        for (int i = 0; i < NCACHELINE; i++){
            pthread_rwlock_rdlock(&cache[i].rwlock);
            if (cache[i].last_used_time < lru) {
                lru = cache[i].last_used_time;
                victim_idx = i;
            }
            pthread_rwlock_unlock(&cache[i].rwlock);
        }

    }

    // write to victim line
    pthread_rwlock_wrlock(&cache[victim_idx].rwlock);
    strcpy(cache[victim_idx].name, uri);
    memcpy(cache[victim_idx].data, object, size);
    cache[victim_idx].datasize = size;
    cache[victim_idx].last_used_time = global_time;
    pthread_rwlock_unlock(&cache[victim_idx].rwlock);
    
    printf("write to cache success\n");

}
