#include <stdio.h>              /* Standard input output, printf function family */
#include <stdlib.h>             /* Standard library, malloc, free, exit */
#include <sys/socket.h>         /* System socket */
#include <netinet/in.h>         /* Internet address family */
#include <netinet/ip.h>         /* Internet Protocol family */
#include <netinet/ip_icmp.h>	/* ICMP protocol family */
#include <arpa/inet.h>			/* Internet address family */
#include <ifaddrs.h>			/* Interface address */
#include <errno.h>              /* Error number */
#include <signal.h>             /* Signal handling */
#include <fcntl.h>              /* File control */
#include <sys/time.h>			/* Time */

#include "../include/basic_define.h" /* Basic define, include <sys/type*/
#include "../libft/libft.h"     /* Libft library,*/

/* Typedef sockaddr_in structure */
typedef struct sockaddr_in t_sockaddr_in;

/* Typedef for icmp header structure */
typedef struct icmphdr t_icmphdr;

/* Typedef for ip header structure */
typedef struct iphdr t_iphdr;

/* Typedef for timeval structure */
typedef struct timeval t_timeval;

/* Buffer size */
#define BUFFER_SIZE 1024

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


/**
 * Ping summary structure one for program
*/
typedef struct s_ping_summary 
{
	suseconds_t		min;			/* Minimum time */
	suseconds_t		max;			/* Maximum time */
	suseconds_t		average;		/* Average time, really needed, recompute between each ping sent ? */
	uint32_t		nb_send;		/* Number of ping send */
	uint32_t		nb_rcv;			/* Number of ping received */
	uint32_t		nb_err;			/* Number of ping error/lost */
}	t_ping_sum;

/**
 * Ping state structure for each ping
*/
typedef struct s_ping_state
{
	suseconds_t		send_time;		/* Last ping send time */
	suseconds_t		rcv_time;		/* Last ping receive time */
}	t_ping_state;

/**
 * Context structure for ping
*/
typedef struct s_context
{
    t_sockaddr_in   dst_sockaddr;	/* Destination socket address */
	t_ping_state    state;          /* ping state */
	t_ping_sum		summary;		/* ping summary*/
    in_addr_t       src_addr;		/* Source address */
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


/* Signal handling global variable */
extern int		g_signal_received;


/* socket handle */
// int				close_socket(int sock);
void			close_multi_socket(int sock1, int sock2);
int				open_rcv_socket(void);
int				open_send_socket(void);

/* listen icmp reply */
void			update_ping_summary(t_ping_sum *sum, suseconds_t start, suseconds_t end);
int8_t			listen_icmp_reply(t_context *c);

/* build request */
t_ping_packet	build_ping_packet(in_addr_t addr_from, in_addr_t addr_dest);

/* checksum */
uint16_t		compute_checksum(uint16_t *data, size_t size);
uint8_t			verify_checksum(void *buffer, uint16_t ip_checksum, uint16_t icmp_checksum);

/* random data */
void			gener_random_data(uint8_t *buff, int size);

/* time */
suseconds_t		get_ms_time(void);

/* network utils */
in_addr_t		ipv4_str_toaddr(char *str);
in_addr_t		get_process_ipv4_addr();