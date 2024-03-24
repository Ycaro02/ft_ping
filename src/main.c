#include <stdio.h>  /* Standard input output, printf function family ... */
#include <stdlib.h> /* Standard library, malloc, free, exit, ... */
#include <sys/socket.h> /* System socket, ... */
#include <netinet/in.h> /* Internet address family, ... */
#include <arpa/inet.h> /* Internet address family, ... */
#include <errno.h> /* Error number, ... */
#include "../include/basic_define.h" /* Basic define, include <sys/type>... */
#include "../libft/libft.h" /* Libft library, ... */


typedef struct sockaddr_in t_sockaddr_in;

struct in_addr str_to_addr(char *str)
{
    struct in_addr addr;

    /* Convert presentation format to binary network format */
    if (inet_pton(AF_INET, str, &addr) <= 0) {
        ft_printf_fd(2, RED"ft_ping: %s: Name or service not known\n"RESET, str);
        exit(1);
    }
    printf("Adresse IP binaire : %08x\n", addr.s_addr);
    return (addr);
}


/* @brief Open socker*/
int open_socket(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    errno = 0;
    if (sock == -1) {
        perror("socket");
        return (-1);
    }
    return (sock);
}

int close_socket(int sock)
{
    errno = 0;
    if (close(sock) == -1) {
        perror("close");
        return (-1);
    }
    return (1);
}

int bind_socket(int sock, t_sockaddr_in *addr)
{
    errno = 0;
    if (bind(sock, (const struct sockaddr *) addr, sizeof(*addr)) == -1) {
        perror("bind");
        return (-1);
    }
    return (1);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }

    int sock = open_socket();
    t_sockaddr_in addr;

    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = str_to_addr(argv[1]).s_addr;

    if (sock == -1) {
        return (1);
    }
    ft_printf_fd(1, YELLOW"Socket opened fd %d on port %u on %s\n"RESET, sock, ntohs(addr.sin_port), inet_ntoa(addr.sin_addr));
    if (bind_socket(sock, &addr) == -1) {
        close_socket(sock);
        return (1);
    }
    

    close_socket(sock);
    return (0);
}