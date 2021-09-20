/* 
 * proxy.c - A simple proxy support HTTP/1.0,
 * GET request and cache.
 */

#include "csapp.h"
#include "sbuf.h" // for producer-consumer model
#include "cache.h"

#define NTHREADS 4 // the max number of working threads
#define SBUFSIZE 16 // the max number of queueing clients  

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *conn_hdr = "Connection: close\r\n";
static const char *proxy_conn_hdr = "Proxy-Connection: close\r\n";

void *thread(void *vargp);
void doit(int connfd);
int read_reqline(int connfd, rio_t *rio_asserver, char *uri);
void parse_uri(char *uri, char *host, char *port, char *path);
void read_reqheaders(rio_t *rio_asserver, char *bufh, const char *host, const char *port);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void sigint_handler(int sig);


sbuf_t sbuf; // shared buffer of accepted descriptors
cache_t cache; // shared cache
int global_time = 0; // for LRU eviction policy

int main(int argc, char **argv) { 
    int listenfd, connfd;
    char *port;
    struct sockaddr_storage clientaddr; // enough space for any address
    char client_host[MAXLINE], client_port[MAXLINE];
    socklen_t clientlen;
    pthread_t tid;

    Signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE signal(ref. CSAPP P964)
    Signal(SIGINT, sigint_handler); // free the space when terminate

    cache_init(&cache);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = argv[1];
    listenfd = Open_listenfd(port);

    // use producer-consumer model for concurrency, ie. prethreading
    sbuf_init(&sbuf, SBUFSIZE);
    for (int i = 0; i < NTHREADS; i++) 
        Pthread_create(&tid, NULL, thread, NULL);
    
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        global_time++; // each time accepte a connection, increase global time
        Getnameinfo((SA *)&clientaddr, clientlen, client_host, MAXLINE,
                client_port, MAXLINE, 0);
        printf("Connected to (%s: %s)\n", client_host, client_port);

        sbuf_insert(&sbuf, connfd);
    }

    return 0;
}

// thread routine
void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);
        Close(connfd);
    }
    return NULL; // should never reach here 
}

void doit(int connfd) {
    char bufl[MAXLINE], bufh[MAXBUF]; // store request line and headers
    char uri[MAXLINE], host[MAXLINE], port[10], path[MAXLINE];
    int clientfd; // proxy as a client to real server
    int cacheline_idx;
    rio_t rio_asserver, rio_asclient;

    Rio_readinitb(&rio_asserver, connfd);
    // read request from client and build new proxy request
    if (read_reqline(connfd, &rio_asserver, uri) < 0)
        return;

    // if hit cache, send object to client from the cache
    if (is_hit(cache, uri, &cacheline_idx)) {
        cache_serve(cache, cacheline_idx, connfd);
        return;
    }

    // if a miss, go to the server
    parse_uri(uri, host, port, path);
    sprintf(bufl, "GET %s HTTP/1.0\r\n", path); // build new request line
    printf("proxy request: %s", bufl);
    read_reqheaders(&rio_asserver, bufh, host, port); // new request headers
                                                      // stores in bufh

    if ((clientfd = open_clientfd(host, port)) < 0) {
        fprintf(stderr, "open_clientfd error: %s\n", strerror(errno));
        return; // error occours, but should not terminate
    }

    // send request to real server
    printf("sending request to %s ...\n\n", host);
    Rio_writen(clientfd, bufl, strlen(bufl));
    Rio_writen(clientfd, bufh, strlen(bufh));

    // read response of real server
    // then send back to client
    int n, total_size = 0;
    char buf[MAXBUF], object[MAX_OBJECT_SIZE];

    Rio_readinitb(&rio_asclient, clientfd);
    // use readnb rather than readlineb to support both string and binary data
    while (n = Rio_readnb(&rio_asclient, buf, MAXBUF)) {
        if (total_size <= MAX_OBJECT_SIZE)
            // use memcpy but not strcpy
            memcpy(object+total_size, buf, n); // append
        total_size += n;
        Rio_writen(connfd, buf, n); // send back to client
    }

    // if the object is too large, discard it
    if (total_size <= MAX_OBJECT_SIZE)
        write_to_cache(cache, uri, object, total_size);

}

/*
 * read request line and parse it 
 * return:
 *     1 if OK, -1 if not a GET request or malformed
 */
int read_reqline(int connfd, rio_t *rp, char *uri) {
    char buf[MAXLINE], method[MAXLINE], version[MAXLINE];

    if (Rio_readlineb(rp, buf, MAXLINE) == 0)
        return -1; // an empty line

    if (sscanf(buf, "%s %s %s", method, uri, version) < 3) {
        clienterror(connfd, buf, "400", "Bad Request",
                "This request is malformed");
        return -1;
    }
    if (strcmp(method, "GET")) {
        clienterror(connfd, method, "501", "Not Implemented",
                "Proxy server dosen't implement this method");
        return -1;
    }
    return 1;
}

/*
 * parse URI into host, port and path
 * note: without error cheking
 */
void parse_uri(char *uri, char *host, char *port, char *path) {
    char *pos1, *pos2; 
    char uri_cpy[MAXLINE];

    strcpy(uri_cpy, uri);
    pos1 = strstr(uri_cpy, "http://");
    if (!pos1) 
        pos1 = uri_cpy;
    else pos1 = uri_cpy + strlen("http://");

    // retrieve path 
    pos2 = strstr(pos1, "/");
    if (pos2) {
        strcpy(path, pos2);
        *pos2 = '\0'; // truncate uri
    }
    else strcpy(path, "/"); // default path

    // retrieve port 
    pos2 = strstr(pos1, ":");
    if (pos2) {
        strcpy(port, pos2+1);
        *pos2 = '\0';
    }
    else strcpy(port, "80"); // default port

    // remaining is host 
    strcpy(host, pos1);

}

/*
 * read HTTP request headers, then build new headers into bufh
 */
void read_reqheaders(rio_t *rp, char *bufh, const char *host, const char *port) {
    char buf[MAXLINE];

    while (Rio_readlineb(rp, buf, MAXLINE) > 0) {
        if (!strcmp(buf, "\r\n"))
            break;

        // skip paticular headers and manually add soon
        if (strstr(buf, "Connection:"))
            continue;
        if (strstr(buf, "User-Agent:"))
            continue;
        if (strstr(buf, "Proxy-Connection:"))
            continue;
        strcat(bufh, buf);
        printf("%s", buf);
    }

    // build new request headers
    if (!strstr(bufh, "Host:")) {
        char host_hdr[MAXLINE];
        sprintf(host_hdr, "Host: %s:%s\r\n", host, port);
        strcat(bufh, host_hdr);
    }
    strcat(bufh, user_agent_hdr);
    strcat(bufh, conn_hdr);
    strcat(bufh, proxy_conn_hdr);
    strcat(bufh, "\r\n");
    
    return;
}

/**
 * common method to return error to client
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char buf[MAXLINE];

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    sprintf(buf, "<html><title>Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=\"ffffff\">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>Proxy server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

/*
 * when terminate, free up the space
 */
void sigint_handler(int sig) {
    int olderrno = errno;

    Sio_puts("Gracefully shutdown...\n");
    Sio_puts("Freeing alloctaed memory...\n");
    sbuf_deinit(&sbuf);
    cache_free(&cache);
    Sio_puts("Done.\n");

    errno = olderrno;
    exit(0);
}

