/* 
 * proxy.c - A simple proxy support HTTP/1.0,
 * GET request and cache.
 */

#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

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




int main(int argc, char **argv) { 
    int listenfd, *connfdp;
    char *port;
    struct sockaddr_storage clientaddr; // enough space for any address
    char client_host[MAXLINE], client_port[MAXLINE];
    socklen_t clientlen;
    pthread_t tid;


    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = argv[1];
    
    listenfd = Open_listenfd(port);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, client_host, MAXLINE,
                client_port, MAXLINE, 0);
        printf("Connected to (%s: %s)\n", client_host, client_port);

        // create a new thread to serve correspond client
        Pthread_create(&tid, NULL, thread, connfdp);
    }

    return 0;
}

// thread routine
void *thread(void *vargp) {
    Pthread_detach(pthread_self());
    int connfd = *((int *)vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}

void doit(int connfd) {
    char bufl[MAXLINE], bufh[MAXBUF]; // store request line and headers
    char uri[MAXLINE], host[MAXLINE], port[10], path[MAXLINE];
    int clientfd; // proxy as a client to real server
    rio_t rio_asserver, rio_asclient;

    Rio_readinitb(&rio_asserver, connfd);

    // read request from client and build new proxy request
    if (read_reqline(connfd, &rio_asserver, uri) < 0)
        return;
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
    // and send back to client
    int n;
    char buf[MAXLINE];
    Rio_readinitb(&rio_asclient, clientfd);
    printf("proxy recieved from real server:\n");
    while (n = Rio_readlineb(&rio_asclient, buf, MAXLINE)) {
        printf("%s", buf);
        Rio_writen(connfd, buf, n); // send back to client
    }


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
    printf("client request:\n");
    printf("%s", buf);

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
 * side effect: will destruct uri
 * note: without error cheking
 */
void parse_uri(char *uri, char *host, char *port, char *path) {
    char *pos1, *pos2; 
    
    pos1 = strstr(uri, "http://");
    if (!pos1) 
        pos1 = uri;
    else pos1 = uri + strlen("http://");

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

