#include <stdio.h>              /* Standard input output, printf function family */
#include <stdlib.h>             /* Standard library, malloc, free, exit */
#include <sys/socket.h>         /* System socket */
#include <netinet/in.h>         /* Internet address family */
#include <netinet/ip.h>         /* Internet Protocol family */
#include <netinet/ip_icmp.h>    /* ICMP protocol family */
#include <arpa/inet.h>          /* Internet address family */
#include <errno.h>              /* Error number */
#include <signal.h>            /* Signal handling */

#include "../include/basic_define.h" /* Basic define, include <sys/type*/
#include "../libft/libft.h"     /* Libft library,*/

/* @brief Define a type for sockaddr_in */
typedef struct sockaddr_in t_sockaddr_in;

/* @brief Define a type for icmphdr ICMP header */
typedef struct icmphdr t_icmphdr;

typedef struct iphdr t_iphdr;


extern int  g_signal_received;

/* ICMP packet size */
#define PACKET_SIZE 84

/* IP header structure size */
#define IP_HDR_SIZE sizeof(struct iphdr)

/* ICMP header structure size */
#define ICMP_HDR_SIZE sizeof(t_icmphdr)

/* ICMP Payload/data size */
#define ICMP_DATA_SIZE (PACKET_SIZE - IP_HDR_SIZE - ICMP_HDR_SIZE)

/* Size of int16 in bits */
#define	SHORT_INT_BITS	16

typedef struct s_ping_packet
{
    t_iphdr    iphdr;
    t_icmphdr  icmphdr;
    char       data[ICMP_DATA_SIZE];
} t_ping_packet;

/* socket handle */
int     bind_socket(int sock, t_sockaddr_in *addr);
int     close_socket(int sock);
int     open_socket(void);

/* listen icmp reply */
void	listen_icmp_reply(int sock);
void	listen_icmp_reply(int sock);

void display_ping_packet(t_ping_packet packet);
t_ping_packet build_ping_packet(char *data, size_t size);