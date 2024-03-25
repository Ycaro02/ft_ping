#include <stdio.h>  /* Standard input output, printf function family ... */
#include <stdlib.h> /* Standard library, malloc, free, exit, ... */
#include <sys/socket.h> /* System socket, ... */
#include <netinet/in.h> /* Internet address family, ... */
#include <netinet/ip.h>         /* Internet Protocol family, ... */
#include <netinet/ip_icmp.h>    /* ICMP protocol family, ... */
#include <arpa/inet.h> /* Internet address family, ... */
#include <errno.h> /* Error number, ... */
#include "../include/basic_define.h" /* Basic define, include <sys/type>... */
#include "../libft/libft.h" /* Libft library, ... */

/* @brief Define a type for sockaddr_in */
typedef struct sockaddr_in t_sockaddr_in;

/* @brief Define a type for icmphdr ICMP header */
typedef struct icmphdr t_icmphdr;

/* socket handle */
int bind_socket(int sock, t_sockaddr_in *addr);
int close_socket(int sock);
int open_socket(void);