#include "../include/ft_ping.h"

/* @brief Open socker*/
int open_socket(void)
{
    int sock;

    errno = 0;
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return (-1);
    }
    return (sock);
}

/* @brief Close socket */
int close_socket(int sock)
{
    errno = 0;
    if (close(sock) == -1) {
        perror("close");
        return (-1);
    }
    return (1);
}

/* @brief Bind socket */
int bind_socket(int sock, t_sockaddr_in *addr)
{
    errno = 0;
    if (bind(sock, (const struct sockaddr *) addr, sizeof(*addr)) == -1) {
        perror("bind");
        return (-1);
    }
    return (1);
}
