#include "../include/ft_ping.h"

int g_signal_received = 0;

/* Ip address string format to bin format */
in_addr_t ipv4_strtoaddr(char *str)
{
    struct in_addr addr;

    /* Convert presentation format to binary network format */
    if (inet_pton(AF_INET, str, &addr) <= 0) {
        ft_printf_fd(2, RED"ft_ping: %s: Name or service not known\n"RESET, str);
        exit(1);
    }
    return (addr.s_addr);
}

/**
 *	@brief Close multiple socket check if socket fd is valid before 
 *	@param sock1 first socket to close
 *	@param sock2 second socket to close
*/
void close_multi_socket(int sock1, int sock2)
{
    if (sock1 != -1) {
        close_socket(sock1);
    }
    if (sock2 != -1) {
        close_socket(sock2);
    }
}

/**
 *	@brief Get process ipv4 address
 *	@return in_addr_t ipv4 address of the process
*/
static in_addr_t get_process_ipv4_addr()
{
    struct ifaddrs *ifa_head, *current;
    in_addr_t addr = 0, local_host = ipv4_strtoaddr(LOCAL_HOST_ADDR);

    errno = 0;
    if (getifaddrs(&ifa_head) == -1) {
        perror("getifaddrs");
        return (0);
    }
    for (current = ifa_head; current != NULL; current = current->ifa_next) {
        if (current->ifa_addr && current->ifa_addr->sa_family == AF_INET) {
            addr = ((struct sockaddr_in *)current->ifa_addr)->sin_addr.s_addr;
            if (addr != local_host) {
                ft_printf_fd(1, PURPLE"Process addr %s\n"RESET, inet_ntoa(*(struct in_addr *)&addr));
                break ;
            }
        }
    }
    freeifaddrs(ifa_head);
    return (addr);
}

/**
 *	@brief Initialize ping context
 *	@param dest_addr destination address to send ping
*/
t_context init_ping_context(char *dest_addr)
{
    t_context c;

    ft_bzero(&c, sizeof(c));
    c.src_addr = get_process_ipv4_addr();
    c.dst_sockaddr.sin_family = AF_INET;
    c.dst_sockaddr.sin_addr.s_addr = ipv4_strtoaddr(dest_addr);
    c.send_sock = open_send_socket();
    c.rcv_sock = open_rcv_socket();
    if (c.send_sock == -1 || c.rcv_sock == -1) {
        close_multi_socket(c.send_sock, c.rcv_sock);
        exit(1);
    }
	c.state.start = get_ms_time();
	usleep(500);
	printf(CYAN"Start time: %ld, diff %ld\n"RESET, c.state.start, get_ms_time() - c.state.start);
    return (c);    
}


void update_ping_state(t_ping_state *state, suseconds_t start, suseconds_t end)
{
	suseconds_t diff = end - start;

	if (state->min == 0 || diff < state->min) {
		state->min = diff;
	}
	if (diff > state->max) {
		state->max = diff;
	}
	state->nb_rcv++;
	state->average = ((state->average * state->nb_rcv) + diff) / state->nb_rcv;
}

/**
 *	@brief Main function
 *	@param argc number of arguments
 *	@param argv arguments
*/
int main(int argc, char **argv)
{
    t_context       c;
    in_addr_t       dest_addr;
    int8_t          ret = 1;
    

    if (argc < 2) {
        ft_printf_fd(2, PURPLE"%s: usage error: Destination address required\n"RESET, argv[0]);
        return (1);
    }
    /* Program can exit here */
    c = init_ping_context(argv[1]);
    dest_addr = c.dst_sockaddr.sin_addr.s_addr;

    t_ping_packet packet = build_ping_packet(c.src_addr, dest_addr);    
    errno = 0;
    ssize_t send_ret = sendto(c.send_sock, &packet, sizeof(packet), 0, (struct sockaddr *)&c.dst_sockaddr, sizeof(c.dst_sockaddr));
    if (send_ret == -1) {
        perror("sendto");
        goto free_socket;
    }
	
	c.state.send_time = get_ms_time();
	c.state.nb_send++;
    ft_printf_fd(1, GREEN"Packet sent %u bytes to %s\n"RESET, send_ret, inet_ntoa(*(struct in_addr *)&dest_addr));

    ret = listen_icmp_reply(&c);

    /* Free socket label */
    free_socket:
    close_multi_socket(c.rcv_sock, c.send_sock);
    ft_printf_fd(1, CYAN"Socket closed return in main\n"RESET);
    
    return (ret);
}