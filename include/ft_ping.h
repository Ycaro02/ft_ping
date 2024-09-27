#include <stdio.h>              /* Standard input output, printf function family (perror)*/
#include <sys/socket.h>         /* System socket (socket) */
#include <netinet/in.h>         /* Internet address family (define IPPROTO_ICMP) */
#include <netinet/ip.h>         /* Internet Protocol family (struct iphdr)*/
#include <netinet/ip_icmp.h>	/* ICMP protocol family (struct iphdr) */
#include <arpa/inet.h>			/* Internet address family (inet_ntoa) */
#include <ifaddrs.h>			/* Interface address (ifaddr func) */
#include <errno.h>              /* Error number */
#include <signal.h>             /* Signal handling (signal) */
#include <fcntl.h>              /* File control (open function)*/
#include <sys/time.h>			/* Time (gettimeofday) */
#include <math.h>				/* Math functions (sqrt)*/

#include <netdb.h>				/* Network database operations (hostent func) */

#include "../libft/libft.h"     /* Libft library,*/

#include "../libft/parse_flag/parse_flag.h" /* Parse flag library */

/* Typedef sockaddr_in structure */
typedef struct sockaddr_in t_sockaddr_in;

/* Typedef for icmp header structure 8 bytes */
typedef struct icmphdr t_icmphdr;

/* Typedef for ip header structure */
typedef struct iphdr t_iphdr;

/* Typedef for timeval structure */
typedef struct timeval t_timeval;

/* Buffer size */
#define BUFF_SIZE 4096

/* ICMP size define */
#define PACKET_SIZE				84					/* ICMP packet size */
#define IP_HDR_SIZE 			sizeof(t_iphdr)		/* IP header structure size 20 bytes */
#define ICMP_HDR_SIZE			sizeof(t_icmphdr)	/* ICMP header structure size */
#define ICMP_DATA_SIZE			56					/* ICMP Payload/data size in bytes */
#define ICMP_TIMESTAMP_SIZE		8					/* Size of timestamp send in bytes */
#define ICMP_BRUT_DATA_BYTES	8					/* Size of brut byte we dont't give to user with pattern options */
#define	SHORT_INT_BITS			16					/* Size of int16 in bits */

/* Explicit value for listen reply function */
#define CONTINUE_LISTEN		0	/* Continue listening reply */
#define STOP_LISTEN			1	/* Stop listening reply */
#define CORRECT_BUFFER		2	/* Correct buffer parse reply */

/* Time define one sec in ms*/
#define ONE_SEC			1000000

/* Local host addr string */
#define LOCAL_HOST_ADDR "127.0.0.1"

/* Help string for mandatory part */
#define MANDATORY_HELP_MESSAGE \
"Usage: %s destination\n\
Options:\n\
  -h --help  \t\tShow this help message and exit\n\
  -v --verbose\t\tVerbose output\n\
  Run make bonus for additional options\n"

/* Help string for bonus part */
#define BONUS_HELP_MESSAGE \
"Usage: %s destination\n\
Options:\n\
  -h --help  \t\tShow this help message and exit\n\
  -v --verbose\t\tVerbose output\n\
  -c --count NUMBER\tStop after sending count ECHO_REQUEST packets\n\
  -t --TTL NUMBER\tChange time to live value only between 1 and 255\n\
  -p --pattern PATTERN\tFill ICMP packet with given pattern (hex)\n\
  -w, --timeout N       stop after N seconds\n\
  -W, --linger N        number of seconds to wait for response\n\
  -V, --version\t\tPrint program version and exit\n\
  "

/* String describing version */
#define VERSION_STRING		CYAN"ft_ping (LIBFT post_common)"RESET" "PURPLE"0.0.1"RESET"\n\
    Reference  "YELLOW"GNU inetutils 2.0."RESET"\n\
    Written by "ORANGE"Ycaro02."RESET"\n\
"

/* Max pattern size accepted for --pattern */
#define MAX_PATTERN_SIZE		(uint32_t)(ICMP_DATA_SIZE - ICMP_TIMESTAMP_SIZE)

/* Ping Flag char */
#define HELP_FLAG_CHAR		'h'		/* 0 help display help */
#define	V_FLAG_CHAR			'v'		/* 1 verbose verbose option */ 
#define	C_FLAG_CHAR			'c'		/* 2 count number of ping send for each target */
#define	T_FLAG_CHAR			't'		/* 3 ttl time to live */
#define P_FLAG_CHAR			'p'		/* 4 pattern fill data with given pattern (hexadecimal only)*/
#define	TIMEOUT_FLAG_CHAR	'w'		/* 5 timeout stop after N seconds */
#define	LINGER_FLAG_CHAR	'W'		/* 6 linger number of seconds to wait for response */
#define VERSION_FLAG_CHAR	'V'		/* 7 version desplay version */

/* Ping flag value */
enum e_ping_flag  {
    UNKNOW=0, 
    HELP_OPTION=(1 << 0),
    V_OPTION=(1 << 1),
	C_OPTION=(1 << 2),
	T_OPTION=(1 << 3),
	P_OPTION=(1 << 4),
	TIMEOUT_OPTION=(1 << 5),
	LINGER_OPTION=(1 << 6),
	VERSION_OPTION=(1 << 7)
};

/**
 * Ping summary structure store all ping summary data on same target, 
 * display and reset between different target
*/
typedef struct s_ping_summary 
{
	t_list			*rcv_time_lst;	/* List of ping receive time (diff between send and receive ts) */
	suseconds_t		min;			/* Minimum time */
	suseconds_t		max;			/* Maximum time */
	suseconds_t		average;		/* Average time */
	suseconds_t		stddev;			/* Standard deviation */
	uint32_t		nb_send;		/* Number of ping send */
	uint32_t		nb_rcv;			/* Number of ping received */
	uint32_t		nb_err;			/* Number of ping error/lost */
}	t_ping_sum;

/**
 * Ping state structure store last ping send and receive time
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
	t_sockaddr_in	sockaddr;		/* Destination address */
	char            *name;			/* Destination name */
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
 * Flag option value structure
*/
typedef struct s_opt_value
{
	char		*pattern;				/* -p Data pattern */
	uint32_t	ttl;					/* -t Tile to live */
	uint32_t	count;					/* -c Count before stop send ping */
	uint8_t		timeout;				/* -w Timeout time in sec before stop program */
	uint8_t		linger;					/* -W Linger, listening reply timeout in sec */
} t_opt_value;

/**
 * Context ping structure
*/
typedef struct s_context
{
	t_ping_state    state;          /* ping state */
	t_ping_sum		summary;		/* ping summary*/
	t_dest_data		dest;			/* current destination data structure */
	t_ping_packet	packet;			/* current packet */
	t_opt_value		opt_value;		/* flag value */
	t_list			*str_args;		/* args (ip addr/hostname) list*/
    in_addr_t       src_addr;		/* Source address */
    int             send_sock;      /* socket for sending */
    int             rcv_sock;       /* socket for receiving */
    uint32_t        flag;           /* ping command flag */
	uint8_t			exit_code;		/* exit code */
	suseconds_t		start;			/* start time */
} t_context;

#ifdef DEBUG
	#define DEBUG_PRINT(fmt, ...) ft_printf_fd(2, fmt, __VA_ARGS__)
#else
	#define DEBUG_PRINT(fmt, ...)
#endif

/* Signal handling global variable */
extern int		g_signal_received;

/* main, special case two different func are define in different main to add specific option */
int8_t			init_flag_context(int argc, char**argv, t_context *c);

/* ping context */
int8_t			call_flag_parser(t_flag_context *flag_c, int argc, char **argv, uint32_t *flag);
t_context		init_ping_context(int argc, char **argv);
void			free_context(t_context *c);

/* send ping */
int				send_ping(t_context *c);
int				sending_ping_loop(t_context *c);

/* summary */
void			display_clear_summary(t_context *c);

/* socket handle */
void			close_multi_socket(int sock1, int sock2);
int				open_rcv_socket(uint8_t linger);
int				open_send_socket(void);

/* listen icmp reply */
int8_t			listen_icmp_reply(t_context *c, int8_t *error);
void			update_ping_summary(t_context *c, suseconds_t start, suseconds_t end);

/* build request */
t_ping_packet	build_ping_packet(t_context *c, in_addr_t addr_from, in_addr_t addr_dest);

/* checksum */
uint16_t		compute_checksum(uint16_t *data, size_t size);
uint8_t			verify_checksum(void *buffer, uint16_t ip_checksum, uint16_t icmp_checksum);

/* random data */
void			gener_random_data(uint8_t *buff, int size);
uint32_t		gener_uint32(int max);

/* time */
suseconds_t		get_ms_time(void);
void			display_ms_time(char *color, suseconds_t time, uint8_t last);

/* network utils */
in_addr_t		ipv4_str_toaddr(char *str);
in_addr_t		get_process_ipv4_addr();
in_addr_t		hostname_to_ipv4_addr(char *hostname);
int8_t			get_destination_addr(char *dest_str, in_addr_t *dest_addr, char **dest_name);

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