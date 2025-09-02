#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void do_something(int connfd);

static void die(const char* m) {
    perror(m);
    exit(1);
}

static void msg(const char* m) {
    perror(m);
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0); // fd being an int referring to something in Linux kernel i.e. TCP protocol, listening port

    int val = 1; 
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)); // SO_REUSEADDR makes a port that is in waiting period (cuz recently TCP closed) to be immediately used 

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = ntohl(0);
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    rv = listen(fd, SOMAXCONN);
    if(rv) {
        die("listen()");
    }

    while(true) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        printf("listening...\n");
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if(connfd < 0) {
            continue; // error
        }
        do_something(connfd);
        close(connfd);
    }
}

static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}