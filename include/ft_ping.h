#include <stdio.h>              /* Standard input output, printf function family */
#include <stdlib.h>             /* Standard library, malloc, free, exit */
#include <sys/socket.h>         /* System socket */
#include <netinet/in.h>         /* Internet address family */
#include <netinet/ip.h>         /* Internet Protocol family */
#include <netinet/ip_icmp.h>	/* ICMP protocol family */
#include <arpa/inet.h>			/* Internet address family */
#include <ifaddrs.h>			/* Interface address (ifaddr func) */
#include <errno.h>              /* Error number */
#include <signal.h>             /* Signal handling */
#include <fcntl.h>              /* File control */
#include <sys/time.h>			/* Time */
#include <math.h>				/* Math functions */

#include <netdb.h>				/* Network database operations (hostent func) */

#include "../libft/libft.h"     /* Libft library,*/

#include "../libft/parse_flag/parse_flag.h" /* Parse flag library */

/* Need to implement send packet to aditional target when sigint is receive*/

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

#define MANDATORY_HELP_MESSAGE \
"Usage: %s destination\n\
Options:\n\
  -h --help  \t\tShow this help message and exit\n\
  -v --verbose\t\tVerbose output\n\
  -c --count value\tStop after sending count ECHO_REQUEST packets\n\
  -t --TTL value\tChange time to live value only between 1 and 255\n"

/* Ping Flag char */
#define H_FLAG_CHAR 'h'         /* 0*/
#define	V_FLAG_CHAR 'v'         /* 1 */ 
#define	C_FLAG_CHAR 'c'			/* 2 */
#define	T_FLAG_CHAR 't'			/* 3 */

#define CORRECT_BUFFER 2U


/* Ping flag value */
enum ping_flag  {
    UNKNOW=0, 
    H_OPTION=1,
    V_OPTION=2,
	C_OPTION=4,
	T_OPTION=8,
};

/**
 * Ping summary structure store all ping summary data on same target, 
 * display and reset between different target
*/
typedef struct s_ping_summary 
{
	t_list			*rcv_time_lst;	/* List of ping time */
	suseconds_t		min;			/* Minimum time */
	suseconds_t		max;			/* Maximum time */
	suseconds_t		average;		/* Average time, really needed, recompute between each ping sent ? */
	suseconds_t		stddev;			/* Standard deviation */
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
 * Destination data structure
*/
typedef struct s_dest_data
{
	t_sockaddr_in	sockaddr;        /* Destination address */
	char            *name;          /* Destination name */
} t_dest_data;


/**
 * Packet structure
*/
typedef struct s_ping_packet
{
    t_iphdr    iphdr;                   /* IP header */
    t_icmphdr  icmphdr;                 /* ICMP header */
    uint8_t    data[ICMP_DATA_SIZE];    /* Data/Payload */
} t_ping_packet;

/**
 * Context ping structure
*/
typedef struct s_context
{
	t_ping_state    state;          /* ping state */
	t_ping_sum		summary;		/* ping summary*/
	t_dest_data		dest;			/* current destination data structure */
	t_ping_packet	packet;			/* current packet */
	t_list			*str_args;		/* args (ip addr/hostname) list*/
    in_addr_t       src_addr;		/* Source address */
    int             send_sock;      /* socket for sending */
    int             rcv_sock;       /* socket for receiving */
    uint16_t        flag;           /* ping command flag */
	uint8_t			exit_code;		/* exit code */
} t_context;




/* Signal handling global variable */
extern int		g_signal_received;

/* main */
int8_t			get_destination_addr(char *dest_str, in_addr_t *dest_addr, char **dest_name);
void			display_clear_summary(t_context *c);

/* send ping */
int				send_ping(t_context *c);
int				sending_ping_loop(t_context *c);

/* socket handle */
void			close_multi_socket(int sock1, int sock2);
int				open_rcv_socket(void);
int				open_send_socket(void);

/* listen icmp reply */
int8_t			listen_icmp_reply(t_context *c, int8_t *error, uint16_t ip_header_id);
void			update_ping_summary(t_context *c, suseconds_t start, suseconds_t end);

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
in_addr_t		hostname_to_ipv4_addr(char *hostname);

/* main */
void			display_ms_time(char *color, suseconds_t time, uint8_t last);

/* detail display */
char			*get_str_msg_type(uint8_t type);
void			display_detail_packet(t_iphdr *ip_hdr, t_icmphdr *icmp_hdr, uint8_t *data);
void			display_ping_summary(t_ping_sum *sum);
void			display_ping_state(t_ping_state *state);
void			display_brut_icmp_data(uint8_t *data, size_t size);
void			display_char_data(uint8_t *data, size_t size);
void			display_detail_iphdr(struct iphdr *header);
void			display_detail_icmphdr(struct icmphdr *header);
void			display_rcv_timelist(t_list *rcv_list);
