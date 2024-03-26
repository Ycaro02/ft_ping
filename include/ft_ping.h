#include <stdio.h>              /* Standard input output, printf function family */
#include <stdlib.h>             /* Standard library, malloc, free, exit */
#include <sys/socket.h>         /* System socket */
#include <netinet/in.h>         /* Internet address family */
#include <netinet/ip.h>         /* Internet Protocol family */
#include <netinet/ip_icmp.h>    /* ICMP protocol family */
#include <arpa/inet.h>          /* Internet address family */
#include <errno.h>              /* Error number */
#include <signal.h>             /* Signal handling */
#include <fcntl.h>              /* File control */
#include "../include/basic_define.h" /* Basic define, include <sys/type*/
#include "../libft/libft.h"     /* Libft library,*/

/* Typedef sockaddr_in structure */
typedef struct sockaddr_in t_sockaddr_in;

/* Typedef for icmp header structure */
typedef struct icmphdr t_icmphdr;

/* Typedef for ip header structure */
typedef struct iphdr t_iphdr;

/* ICMP packet size */
#define PACKET_SIZE 84

/* IP header structure size */
#define IP_HDR_SIZE sizeof(struct iphdr)

/* ICMP header structure size */
#define ICMP_HDR_SIZE sizeof(t_icmphdr)

/* ICMP Payload/data size */
#define ICMP_DATA_SIZE 56

/* Size of int16 in bits */
#define	SHORT_INT_BITS	16

/* Local host addr string */
#define LOCAL_HOST_ADDR "127.0.0.1"

/* Data magic value 'f' */
#define DATA_MBVAL 102
/* Magic value index */
#define DATA_MBIDX 3
/* Padding 0 after magic val */
#define DATA_PAD_ZERO 4
/* End of padding */
#define DATA_END_PAD DATA_MBIDX + DATA_PAD_ZERO

void gener_random_data(uint8_t *buff, int size);

/* First used brut data can be remove */
#define BRUT_DATA \
{ 109, 243, 2, 102, 0, 0, 0, 0, 54, 209, 12, 0, 0, 0, 0, 0, \
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, \
16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, \
32, 33, 34, 35, 36, 37, 38, 39\
}

typedef struct s_context
{
    t_sockaddr_in   dst_sockaddr;  /* Destination socket address */
    in_addr_t       src_addr;    /* Source address */
    int             send_sock;      /* socket for sending */
    int             rcv_sock;       /* socket for receiving */
    uint16_t        flag;           /* ping flag */

} t_context;

/**
 * Packet structure for ping
*/
typedef struct s_ping_packet
{
    t_iphdr    iphdr;                   /* IP header */
    t_icmphdr  icmphdr;                 /* ICMP header */
    uint8_t    data[ICMP_DATA_SIZE];    /* Data/Payload */
} t_ping_packet;


/* Signal handling variable */
extern int  g_signal_received;

/* socket handle */
int     bind_socket(int sock, t_sockaddr_in *addr);
int     close_socket(int sock);
int     open_rcv_socket(void);
int     open_send_socket(void);
/* listen icmp reply */
int8_t listen_icmp_reply(int sock);

/* build request */
void          display_ping_packet(t_ping_packet packet);
t_ping_packet build_ping_packet(in_addr_t addr_from, in_addr_t addr_dest);
/* checksum */
uint16_t    compute_checksum(uint16_t *data, size_t size);
uint8_t     verify_checksum(void *buffer, uint16_t ip_checksum, uint16_t icmp_checksum);
