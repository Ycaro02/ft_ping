#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }

    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        perror("gethostbyname");
        return 1;
    }

    char *ip = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
    printf("Local IP address: %s\n", ip);
    return 0;
}
