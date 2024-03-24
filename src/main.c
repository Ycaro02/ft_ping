#include <stdio.h>  /* Standard input output, printf function family ... */
#include <stdlib.h> /* Standard library, malloc, free, exit, ... */
#include <sys/socket.h> /* System socket, ... */
#include <netinet/in.h> /* Internet address family, ... */
#include "../include/basic_define.h" /* Basic define, include <sys/type>... */
#include "../libft/libft.h" /* Libft library, ... */


typedef struct sockaddr_in t_sockaddr_in;


/* @brief Open socker*/
int open_socket(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return (-1);
    }
    return (sock);
}

int close_socket(int sock)
{
    if (close(sock) == -1) {
        perror("close");
        return (-1);
    }
    return (1);
}

int bind_socket(int sock, t_sockaddr_in *addr)
{
    if (bind(sock, (const struct sockaddr *) addr, sizeof(*addr)) == -1) {
        perror("bind");
        return (-1);
    }
    return (1);
}

int main(void)
{
    int sock = open_socket();
    t_sockaddr_in addr;

    ft_bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (sock == -1) {
        return (1);
    }
    ft_printf_fd(1, "Socket opened fd %d on port 8080, listen on INADDR_ANY\n", sock);
    if (bind_socket(sock, &addr) == -1) {
        close_socket(sock);
        return (1);
    }
    close_socket(sock);
    return (0);
}