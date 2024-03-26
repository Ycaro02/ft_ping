#include "../include/ft_ping.h"

/* @brief Open socker*/
int open_send_socket(void)
{
    int sockfd;

    errno = 0;
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("socket");
        return (-1);
    }

    // Désactiver l'ajout automatique de l'en-tête IP
    int disable = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &disable, sizeof(disable)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    return (sockfd);
}


int open_rcv_socket(void)
{
    int sockfd;

    errno = 0;
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return (-1);
    }
    return (sockfd);
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
